#include <array>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "PerspectiveCamera.h" 
 
namespace Framework {
    PerspectiveCamera::PerspectiveCamera(const Frustrum& frustrum, const glm::vec3& position, const glm::vec3& lookAt, const glm::vec3& upVector) {

        CameraFrustrum = frustrum;
        UpVector = upVector;
        LookAt = lookAt;
        Position = position;
        RecalculateMatrix();
    }   


    void PerspectiveCamera::RecalculateMatrix() {
        
        // Projection
        ProjectionMatrix = glm::perspective(
            CameraFrustrum.angle,
            CameraFrustrum.width / CameraFrustrum.height,
            CameraFrustrum.near,
            CameraFrustrum.far
        );

        // View
        ViewMatrix = glm::lookAt(
            Position,
            LookAt,
            UpVector
        );

        // View-Projection
        ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
    }   
};