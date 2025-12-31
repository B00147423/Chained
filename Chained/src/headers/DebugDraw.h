#pragma once
// DebugDraw.h
#pragma once
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <imgui.h>

class DebugDraw : public b2Draw {
public:
    static constexpr float PHYSICS_SCALE = 32.0f;
    
    DebugDraw() {
        SetFlags(b2Draw::e_shapeBit);
    }

    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {
        ImU32 col = ImGui::GetColorU32(ImVec4(color.r, color.g, color.b, color.a));
        for (int i = 0; i < vertexCount; ++i) {
            const b2Vec2& v1 = vertices[i];
            const b2Vec2& v2 = vertices[(i + 1) % vertexCount];
            // Convert from Box2D meters to screen pixels
            ImVec2 screenV1(v1.x * PHYSICS_SCALE, v1.y * PHYSICS_SCALE);
            ImVec2 screenV2(v2.x * PHYSICS_SCALE, v2.y * PHYSICS_SCALE);
            ImGui::GetBackgroundDrawList()->AddLine(screenV1, screenV2, col, 2.0f);
        }
    }

    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override {
        DrawPolygon(vertices, vertexCount, color);
    }

    void DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override {
        ImU32 col = ImGui::GetColorU32(ImVec4(color.r, color.g, color.b, color.a));
        // Convert from Box2D meters to screen pixels
        ImVec2 screenCenter(center.x * PHYSICS_SCALE, center.y * PHYSICS_SCALE);
        float screenRadius = radius * PHYSICS_SCALE;
        ImGui::GetBackgroundDrawList()->AddCircle(screenCenter, screenRadius, col, 32, 2.0f);
    }

    void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override {
        DrawCircle(center, radius, color);
    }

    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override {
        ImU32 col = ImGui::GetColorU32(ImVec4(color.r, color.g, color.b, color.a));
        // Convert from Box2D meters to screen pixels
        ImVec2 screenP1(p1.x * PHYSICS_SCALE, p1.y * PHYSICS_SCALE);
        ImVec2 screenP2(p2.x * PHYSICS_SCALE, p2.y * PHYSICS_SCALE);
        ImGui::GetBackgroundDrawList()->AddLine(screenP1, screenP2, col, 2.0f);
    }

    void DrawTransform(const b2Transform& xf) override {
        // Optional: draw axis lines for body transforms
    }
};
