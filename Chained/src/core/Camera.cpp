#include "../headers/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
namespace Chained {

    Camera::Camera(float viewportWidth, float viewportHeight)
        : m_viewportWidth(viewportWidth), m_viewportHeight(viewportHeight) {
    }

    void Camera::setViewport(float width, float height) {
        m_viewportWidth = width;
        m_viewportHeight = height;
    }

    void Camera::setPostion(const glm::vec2& pos) {
        m_position = pos;
    }

    void Camera::setZoom(float zoom) {
        m_zoom = glm::clamp(zoom, 0.05f, 100.0f);
    }

    void Camera::pan(const glm::vec2& delta) {
        m_position += delta / m_zoom;
    }

    void Camera::zoomAtScreen(float zoomFactor, const glm::vec2& screenPoint) {
        float prevZoom = m_zoom;
        glm::vec2 before = screenToWorld(screenPoint);
        m_zoom = glm::clamp(m_zoom * zoomFactor, 0.05f, 100.0f);
        glm::vec2 after = screenToWorld(screenPoint);
        m_position += before - after;
    }

    glm::mat4 Camera::getProjectionMatrix() const {
        float left = m_position.x;
        float right = m_position.x + m_viewportWidth / m_zoom;
        float bottom = m_position.y;
        float top = m_position.y + m_viewportHeight / m_zoom;
        return glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
    }

    glm::vec2 Camera::screenToWorld(const glm::vec2& screen) const {
        // For orthographic projection, convert screen coordinates to world coordinates
        // screen: (0,0) is top-left of window
        // world: camera position is at top-left of visible area
        return m_position + (screen / m_zoom);
    }

    glm::vec2 Camera::worldToScreen(const glm::vec2& world) const {
        return (world - m_position) * m_zoom;
    }

} 
