#ifndef RAYCASTER_HPP
#define RAYCASTER_HPP

#include "Vector2.hpp"

class Raycaster
{
public:
  int mapWidth;
  int mapHeight;
  int *worldMap;

  Raycaster(int mapWidth, int mapHeight, int *worldMap);

  void SetCameraPosition(Vector2 position);
  void SetCameraDirection(Vector2 direction);
  void SetCameraDirection(double angleInRadians);
  void SetFOVInRadians(double fovInRadians);
  void SetClipPlaneDistance(double distance);
  void RenderToBuffer(int width, int height, unsigned char *buffer);

private:
  Vector2 cameraPosition;
  Vector2 cameraDirection;
  Vector2 clipPlaneRightVector;
  double fovInRadians;
  double distanceToClipPlane;

  void UpdateClipPlaneVector(void);
  bool IsWallAtMapPosition(int xPos, int yPos);

  enum Side {
    NorthSouth,
    EastWest
  };
};

#endif