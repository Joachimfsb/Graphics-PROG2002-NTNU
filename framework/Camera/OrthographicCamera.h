#ifndef ORTHOGRAPHICCAMERA_H_
#define ORTHOGRAPHICCAMERA_H_

#include "Camera.h"

#include <glm/fwd.hpp>

#include <array>

namespace Framework {
  class OrthographicCamera : public Camera
  {
  public:

    struct Frustrum {
      float left;
      float right;
      float bottom;
      float top;
      float near;
      float far;
    };

  public:
    OrthographicCamera(const Frustrum& frustrum = {-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f},
                      const glm::vec3& position = glm::vec3(0.0f), float rotation = 0.0f);
    ~OrthographicCamera() = default;

    OrthographicCamera(const OrthographicCamera& camera) : Camera(camera)
    {
      this->Rotation = camera.Rotation;
      this->CameraFrustrum = camera.CameraFrustrum;
    }

    void SetRotation(float rotation)
    { this->Rotation = rotation; this->RecalculateMatrix(); }

    void SetFrustrum(const Frustrum& frustrum)
    { this->CameraFrustrum =frustrum; this->RecalculateMatrix(); }

  protected:
    void RecalculateMatrix();

  protected:
    float Rotation;
    Frustrum CameraFrustrum;
  };
};

#endif // ORTHOGRAPHICCAMERA_H_
