#include "Raycaster.hpp"

#include <cmath>
#include <string.h>

Raycaster::Raycaster(int mapWidth, int mapHeight, int *worldMap)
{
  this->mapWidth = mapWidth;
  this->mapHeight = mapHeight;
  this->worldMap = worldMap;

  fovInRadians = M_PI / 3.0;
  distanceToClipPlane = 1.0;
}

void Raycaster::SetCameraPosition(Vector2 position)
{
  cameraPosition = position;
}

void Raycaster::SetCameraDirection(Vector2 direction)
{
  cameraDirection.CloneFrom(direction);
  cameraDirection.Normalise();
  UpdateClipPlaneVector();
}

void Raycaster::SetCameraDirection(double angleInRadians)
{
  cameraDirection.x = 1.0;
  cameraDirection.y = 0.0;
  cameraDirection.Rotate(angleInRadians);
  UpdateClipPlaneVector();
}

void Raycaster::SetFOVInRadians(double fovInRadians)
{
  this->fovInRadians = fovInRadians;
}

void Raycaster::SetClipPlaneDistance(double distance)
{
  distanceToClipPlane = distance;
}

void Raycaster::UpdateClipPlaneVector(void)
{
  clipPlaneRightVector.x = cameraDirection.y;
  clipPlaneRightVector.y = -cameraDirection.x;
  clipPlaneRightVector.Scale(distanceToClipPlane * tan(fovInRadians / 2.0));
}

void Raycaster::RenderToDisplay(DisplayWrapper *display, unsigned char* textureData)
{
  const unsigned int displayWidth = display->GetWidth();
  const unsigned int displayHeight = display->GetHeight();
  unsigned char *displayBuffer = display->GetBuffer();

  const int bufferSize = displayWidth * displayHeight;

  // zero display (i.e. black floor & ceiling)
  memset(displayBuffer, 0, bufferSize);

  for (int x = 0; x < displayWidth; x++)
  {
    int mapX = int(cameraPosition.x);
    int mapY = int(cameraPosition.y);

    // camera space: -1.0  at left of screen, +1.0 at right
    double cameraX = 2.0 * x / double(displayWidth) - 1;

    // calculate ray vector: from camera position to intersecting point on clip plane
    Vector2 ray = Vector2(clipPlaneRightVector.x, clipPlaneRightVector.y);
    ray.Scale(cameraX);
    Vector2 vectorToClipPlanceCentre;
    vectorToClipPlanceCentre.CloneFrom(cameraDirection);
    vectorToClipPlanceCentre.Scale(distanceToClipPlane);
    ray.Add(vectorToClipPlanceCentre);

    double lengthOfRayToNextXBoundary;
    double lengthOfRayToNextYBoundary;

    double lengthOfRayToCrossOneGridWidth = std::abs(1.0 / ray.x);
    double lengthOfRayToCrossOneGridHeight = std::abs(1.0 / ray.y);

    double perpendicularWallDistance; // distance to wall projected onto the camera direction vector
    int signX;
    int signY;

    bool hit = false;
    Side side;

    if (ray.x < 0)
    {
      signX = -1;
      lengthOfRayToNextXBoundary = (cameraPosition.x - mapX) * lengthOfRayToCrossOneGridWidth;
    } 
    else
    {
      signX = 1;
      lengthOfRayToNextXBoundary = (mapX + 1.0 - cameraPosition.x) * lengthOfRayToCrossOneGridWidth;
    }
    if (ray.y < 0)
    {
      signY = -1;
      lengthOfRayToNextYBoundary = (cameraPosition.y - mapY) * lengthOfRayToCrossOneGridHeight;
    } 
    else
    {
      signY = 1;
      lengthOfRayToNextYBoundary = (mapY + 1.0 - cameraPosition.y) * lengthOfRayToCrossOneGridHeight;
    }

    while (!hit)
    {
      if (lengthOfRayToNextXBoundary < lengthOfRayToNextYBoundary)
      {
        lengthOfRayToNextXBoundary += lengthOfRayToCrossOneGridWidth;
        mapX += signX;
        side = NorthSouth;
      }
      else
      {
        lengthOfRayToNextYBoundary += lengthOfRayToCrossOneGridHeight;
        mapY += signY;
        side = EastWest;
      }
      if (IsWallAtMapPosition(mapX, mapY))
        hit = true;
    }

    double textureUV_U;
    // (1 - signX) >> 1)  : add 1 only if the sign is -ve
    switch (side)
    {
    case NorthSouth:
      perpendicularWallDistance = (mapX - cameraPosition.x + ((1 - signX) >> 1)) / ray.x;
      textureUV_U = cameraPosition.y + perpendicularWallDistance * ray.y;
      break;
    case EastWest:
      perpendicularWallDistance = (mapY - cameraPosition.y + ((1 - signY) >> 1)) / ray.y;
      textureUV_U = cameraPosition.x + perpendicularWallDistance * ray.x;
      break;
    }
    textureUV_U -= floor(textureUV_U);

    if (perpendicularWallDistance > (mapWidth + mapHeight))
      perpendicularWallDistance = mapWidth + mapHeight;

    // int lineHeight = (int)displayHeight;
    // if (perpendicularWallDistance > 1)
    //   lineHeight = (int)(displayHeight / perpendicularWallDistance);
    unsigned int lineHeight = (int)(displayHeight / perpendicularWallDistance);

//    int textureNum = 0;
    int textureColumn = int(textureUV_U * 32.0); // assumes textures are 32px wide

    // if(side == 0 && rayDirX > 0) texX = texWidth - texX - 1;
    // if(side == 1 && rayDirY < 0) texX = texWidth - texX - 1;

    // picks shades from 100 to 250 (to implement shading later)
    double shade = 1.0; // 100 + (*(worldMap + (mapY * displayWidth) + mapX) * 30);
    if (side == EastWest) 
      shade = 0.75; // darken north-south walls

    shade /= (perpendicularWallDistance < 3.0) ? 1.0 : perpendicularWallDistance * 0.333;

    int startPixelY = (displayHeight - lineHeight);
    if (startPixelY < 0)
      startPixelY = 0;
    startPixelY >>= 1;
    int endPixelY = 63 - startPixelY;

    double textureRow;
    double textureRowStep;
    if (lineHeight < displayHeight)
    {
      textureRow = 0;
      textureRowStep = 32.0 / (double)(endPixelY - startPixelY);
    }
    else
    {
      const double amountVisible = ((double)displayHeight / (double)lineHeight);
      textureRow = 32.0 * (1.0 - amountVisible) / 2.0;
      textureRowStep = (32.0 * amountVisible) / 64.0;
    }

    for (int y = startPixelY; y <= endPixelY; y++)
    {
      unsigned char texel = *(textureData + ((int)(textureRow) << 5) + textureColumn);
      textureRow += textureRowStep;
      int offset = (y << 7) + x; // (y >> 7) assuming displayWidth = 128 !!
      if (offset < bufferSize)
      *(displayBuffer + offset) = texel * shade;
    }
  }
}

bool Raycaster::IsWallAtMapPosition(int xPos, int yPos)
{
  if (xPos < 0 || xPos >= mapWidth || yPos < 0 || yPos >= mapHeight)
    return true;

  return (*(worldMap + (yPos * mapWidth) + xPos) > 0);
}

