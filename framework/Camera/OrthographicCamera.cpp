#include <array>
#include <glm/fwd.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "OrthographicCamera.h" 
 
namespace Framework {
    OrthographicCamera::OrthographicCamera(const Frustrum& frustrum, const glm::vec3& position, float rotation) {

        CameraFrustrum = frustrum;
        Rotation = rotation;
        Position = position;
        RecalculateMatrix();
    }   


    void OrthographicCamera::RecalculateMatrix() {
        
        // Projection
        ViewProjectionMatrix = glm::ortho(
            CameraFrustrum.left,
            CameraFrustrum.right,
            CameraFrustrum.bottom,
            CameraFrustrum.top,
            CameraFrustrum.near,
            CameraFrustrum.far
        );

        // View
        ViewMatrix = glm::translate(glm::mat4(1.0f), Position)
                * glm::rotate(glm::mat4(1.0f), glm::radians(Rotation), {0.0f, 0.0f, 1.0f});

        // View-Projection
        ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;
    }   
};