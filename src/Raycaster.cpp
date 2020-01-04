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

void Raycaster::RenderToBuffer(int width, int height, unsigned char *buffer)
{
  const int bufferSize = width * height;
  // zero display (i.e. black floor & ceiling)
  memset(buffer, 0, bufferSize);

  for (int x = 0; x < width; x++)
  {
    int mapX = int(cameraPosition.x);
    int mapY = int(cameraPosition.y);

    // camera space: -1.0  at left of screen, +1.0 at right
    double cameraX = 2.0 * x / double(width) - 1;

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

    // (1 - signX) >> 1)  : add 1 only if the sign is -ve
    switch (side)
    {
    case NorthSouth:
      perpendicularWallDistance = (mapX - cameraPosition.x + ((1 - signX) >> 1)) / ray.x;
      break;
    case EastWest:
      perpendicularWallDistance = (mapY - cameraPosition.y + ((1 - signY) >> 1)) / ray.y;
      break;
    }

    if (perpendicularWallDistance > (mapWidth + mapHeight))
      perpendicularWallDistance = mapWidth + mapHeight;

    int lineHeight = (int)height;
    if (perpendicularWallDistance > 1)
      lineHeight = (int)(height / perpendicularWallDistance);

    // picks shades from 100 to 250 (to implement shading later)
    unsigned char shade = 255; // 100 + (*(worldMap + (mapY * width) + mapX) * 30);
    if (side == EastWest) 
      shade = 192; // darken north-south walls
    
    int startPixelY = (height - lineHeight) >> 1;
    for (int y = startPixelY; y < startPixelY + lineHeight; y++)
    {
      int offset = (y << 7) + x; // (y >> 7) assuming width = 128 !!
      if (offset < bufferSize)
      *(buffer + offset) = shade;
    }
  }
}

bool Raycaster::IsWallAtMapPosition(int xPos, int yPos)
{
  if (xPos < 0 || xPos >= mapWidth || yPos < 0 || yPos >= mapHeight)
    return true;

  return (*(worldMap + (yPos * mapWidth) + xPos) > 0);
}

