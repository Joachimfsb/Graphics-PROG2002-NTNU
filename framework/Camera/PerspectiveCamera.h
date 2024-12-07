#ifndef PERSPECTIVECAMERA_H_
#define PERSPECTIVECAMERA_H_

#include "Camera.h"

#include <array>

namespace Framework {
  class PerspectiveCamera : public Camera
  {
  public:

    struct Frustrum {
      float angle;
      float width;
      float height;
      float near;
      float far;
    };

  public:
    PerspectiveCamera(const Frustrum& frustrum = {45.0f, -1.0f, 1.0f, 1.0f, -1.0f},
                      const glm::vec3& position = glm::vec3(0.0f),
                      const glm::vec3& lookAt = glm::vec3(-1.0f),
                      const glm::vec3& upVector = glm::vec3(0.0f, 1.0f, 0.0f));
    ~PerspectiveCamera() = default;
    PerspectiveCamera(const PerspectiveCamera& camera) : Camera(camera)
    {
      this->LookAt = camera.LookAt;
      this->UpVector = camera.UpVector;
      this->CameraFrustrum = camera.CameraFrustrum;
    }

    const Frustrum GetFrustrum() const { return CameraFrustrum; }
    const glm::vec3 GetLookAt() const { return LookAt; }
    const glm::vec3 GetUpVector() const { return UpVector; }

    void SetFrustrum(const Frustrum& frustrum)
    { this->CameraFrustrum = frustrum; this->RecalculateMatrix(); }

    void SetLookAt(const glm::vec3& lookAt)
    { this->LookAt = lookAt; this->RecalculateMatrix(); }

    void SetUpVector(const glm::vec3& upVector)
    { this->UpVector = upVector; this->RecalculateMatrix(); }

  protected:
    void RecalculateMatrix();

  protected:
    glm::vec3 LookAt;
    glm::vec3 UpVector;
    Frustrum CameraFrustrum;
  };
}

#endif // PERSPECTIVECAMERA_H_
