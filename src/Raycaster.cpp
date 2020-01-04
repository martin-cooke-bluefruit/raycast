#include "Raycaster.hpp"

#include <cmath>
#include <string.h>

Raycaster::Raycaster(int mapWidth, int mapHeight, int *worldMap)
{
  this->mapWidth = mapWidth;
  this->mapHeight = mapHeight;
  this->worldMap = worldMap;
}

void Raycaster::SetCameraPosition(Vector2 position)
{
  cameraPosition = position;
}

void Raycaster::SetCameraDirection(Vector2 direction)
{
  cameraDirection = direction;
  UpdateClipPlaneVector();
}

void Raycaster::SetCameraDirection(double angleInRadians)
{
  cameraDirection.x = 1.0;
  cameraDirection.y = 0.0;
  cameraDirection.Rotate(angleInRadians);
  UpdateClipPlaneVector();
}

void Raycaster::RenderToBuffer(int width, int height, unsigned char *buffer)
{
  // clear display
  memset(buffer, 0, width * height);

  for (int x = 0; x < width; x++)
  {
    int mapX = int(cameraPosition.x);
    int mapY = int(cameraPosition.y);

    // camera space: -1.0  at left of screen, +1.0 at right
    double cameraX = 2.0 * x / double(width) - 1;

    // calculate ray vector: from camera position to intersecting point on clip plane
    Vector2 ray = Vector2(clipPlaneRightVector.x, clipPlaneRightVector.y);
    ray.Scale(cameraX);
    ray.Add(cameraDirection);

    double lengthOfRayToNextXBoundary;
    double lengthOfRayToNextYBoundary;

    double lengthOfRayToCrossOneGridWidth = std::abs(1.0 / ray.x);
    double lengthOfRayToCrossOneGridHeight = std::abs(1.0 / ray.y);

    double perpendicularWallDistance; // distance to wall projected onto the camera direction vector
    int signX;
    int signY;

    int hit = 0;
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

    while (hit == 0)
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
      if (*(worldMap + (mapY * width) + mapX) > 0)
        hit = 1;
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
      shade = 200; // darken north-south walls

    int startPixelY = (height - lineHeight) >> 1;
    for (int y = startPixelY; y < startPixelY + lineHeight; y++)
      *(buffer + (y << 7) + x) = shade; // (y >> 7) assuming width = 128 !!
  }
}

void Raycaster::UpdateClipPlaneVector(void)
{
  clipPlaneRightVector.x = cameraDirection.y;
  clipPlaneRightVector.y = -cameraDirection.x;
  clipPlaneRightVector.Scale(0.66);
}
