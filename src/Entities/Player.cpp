#include "Entities/Player.h"
#include "Utility/Timer.h"
#include "raylib.h"
#include "raymath.h"

namespace Entities {
Player::Player() {
  // PlayerCube init
  playerCube.color = GREEN;
  playerCube.width = 1.0f;
  playerCube.height = 2.0f;
  playerCube.length = 1.0f;
  playerCube.position = (Vector3){0, playerCube.height / 2, 4}; // spawn

  // Camera init
  camera = {0};
  camera.position = Vector3Add(
      playerCube.position, (Vector3){0, playerCube.height / 2.0f - 0.2f, 0});
  camera.target = Vector3Zero();           // camera looking at point
  camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // camera up vector
  camera.fovy = 90.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  cameraMode = CAMERA_FIRST_PERSON;
  DisableCursor(); // limit cursor movement to window

  isShooting = false;
}

void Player::Event() {
  // TODO input events
  if (IsKeyPressed(KEY_THREE)) {
    cameraMode = CAMERA_THIRD_PERSON;
    camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // reset roll
  }

  if (IsKeyPressed(KEY_ONE)) {
    cameraMode = CAMERA_FIRST_PERSON;
    camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // reset roll
  }

  if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    isShooting = true;
    timer.Start(0.1f);
  }
}

void Player::Update() {
  // Update hitscan timer
  timer.Update();
  if (timer.Finished()) {
    isShooting = false;
  }
  // UpdateCamera(&camera, cameraMode);
  UpdateCameraPro(
      &camera,
      (Vector3){
          (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) *
                  0.1f - // Move forward-backward
              (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) * 0.1f,
          (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) * 0.1f - // Move right-left
              (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) * 0.1f,
          0.0f // Move up-down
      },
      (Vector3){
          GetMouseDelta().x * 0.05f, // Rotation: yaw
          GetMouseDelta().y * 0.05f, // Rotation: pitch
          0.0f                       // Rotation: roll
      },
      GetMouseWheelMove() * 2.0f); // Move to target (zoom)

  // Update camera target to be a point directly in fron of camera
  Vector3 forward =
      Vector3Normalize(Vector3Subtract(camera.target, camera.position));
  camera.target = Vector3Add(camera.position, Vector3Scale(forward, 100.0f));

  // Update position
  if (cameraMode == CAMERA_FIRST_PERSON) {
    playerCube.position.x = camera.position.x;
    playerCube.position.z = camera.position.z;
  }

  if (cameraMode == CAMERA_THIRD_PERSON) {
    playerCube.position.x = camera.target.x;
    playerCube.position.z = camera.target.z;
  }
}

void Player::Draw() {
  BeginMode3D(camera);
  DrawCube(playerCube.position, playerCube.width, playerCube.height,
           playerCube.length, playerCube.color);
  DrawCubeWires(playerCube.position, playerCube.width, playerCube.height,
                playerCube.length, DARKPURPLE);
  if (isShooting) {
    Ray hitscanRay = {
        camera.position,
        Vector3Normalize(Vector3Subtract(camera.target, playerCube.position))};
    DrawRay(hitscanRay, RED);
  }
  EndMode3D();
}

Vector3 Player::GetPosition() { return playerCube.position; }

Ray Player::GetRay() {
  Ray hitscanRay = {camera.position, Vector3Normalize(Vector3Subtract(
                                         camera.target, playerCube.position))};
  return hitscanRay;
}
} // namespace Entities