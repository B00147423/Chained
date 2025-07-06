#pragma once
#include <glm/glm.hpp>


namespace Chained {
    class Camera {
    public:
        Camera(float viewportWidth, float viewportHeight);
        void setViewport(float width, float height);
        void setPostion(const glm::vec2& pos);
        void setZoom(float zoom);
        void pan(const glm::vec2& delta);
        void zoomAtScreen(float zoomfactor, const glm::vec2& screenPoint);

        glm::mat4 getProjectionMatrix() const;
        glm::vec2 screenToWorld(const glm::vec2& screen) const;
        glm::vec2 worldToScreen(const glm::vec2& world) const;

        // --- Public getters (all inline, no Camera:: needed here!) ---
        float getZoom() const { return m_zoom; }
        const glm::vec2& getPosition() const { return m_position; }
        float getViewportWidth() const { return m_viewportWidth; }
        float getViewportHeight() const { return m_viewportHeight; }

    private:
        glm::vec2 m_position = { 0.f, 0.f }; // Top-left of camera in world space
        float m_zoom = 1.0f;
        float m_viewportWidth;
        float m_viewportHeight;
    };
}