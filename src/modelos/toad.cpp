#include "modelos/toad.hpp"

// Utility to create a colored part
static Objeto3D* createPart(Objeto3D* shape, float tx, float ty, float tz, float sx, float sy, float sz, float r, float g, float b) {
    shape->setTranformacion(_vertex4<float>(Translacion_, tx, ty, tz));
    shape->setTranformacion(_vertex4<float>(Escalado_, sx, sy, sz));
    shape->setColor(_vertex3<float>(r, g, b));
    shape->setAmb(_vertex4<float>(r * 0.4f, g * 0.4f, b * 0.4f, 1.0f));
    shape->setDiff(_vertex4<float>(r, g, b, 1.0f));
    return shape;
}

Toad3D::Toad3D(const std::string & filename) : Objeto3D() {
    // -------------------------------------------------------------------------
    // BODY
    // -------------------------------------------------------------------------
    // Pants (White/Rounded)
    setHijo(createPart(new EsferaObj3D(), 0.0, 0.4, 0.0, 0.5, 0.5, 0.5, 1.0, 1.0, 1.0));
    
    // Vest (Blue with Gold trim)
    setHijo(createPart(new CuboObj3D(), 0.0, 0.95,-0.05, 0.7, 0.6, 0.5, 0.1, 0.3, 0.9));
    setHijo(createPart(new CuboObj3D(), -0.36, 0.95, 0.22, 0.05, 0.6, 0.05, 1.0, 0.9, 0.0)); // Left Gold
    setHijo(createPart(new CuboObj3D(),  0.36, 0.95, 0.22, 0.05, 0.6, 0.05, 1.0, 0.9, 0.0)); // Right Gold

    // Arms (Skin tone)
    setHijo(createPart(new CuboObj3D(), -0.45, 0.9, 0.0, 0.2, 0.6, 0.2, 1.0, 0.8, 0.6));
    setHijo(createPart(new CuboObj3D(),  0.45, 0.9, 0.0, 0.2, 0.6, 0.2, 1.0, 0.8, 0.6));

    // -------------------------------------------------------------------------
    // HEAD & FACE (Improved Clarity)
    // -------------------------------------------------------------------------
    // Head Base (Skin tone, moved forward and sized for clarity)
    // Toad has a very rounded, almost cylindrical head
    setHijo(createPart(new EsferaObj3D(), 0.0, 1.45, 0.1, 0.55, 0.5, 0.5, 1.0, 0.8, 0.6));
    
    // HUGE VERTICAL EYES
    setHijo(createPart(new EsferaObj3D(), -0.18, 1.55, 0.52, 0.06, 0.18, 0.06, 0.02, 0.02, 0.02)); // Left
    setHijo(createPart(new EsferaObj3D(), -0.18, 1.65, 0.58, 0.02, 0.04, 0.01, 1.0, 1.0, 1.0)); // Sparkle
    
    setHijo(createPart(new EsferaObj3D(),  0.18, 1.55, 0.52, 0.06, 0.18, 0.06, 0.02, 0.02, 0.02)); // Right
    setHijo(createPart(new EsferaObj3D(),  0.18, 1.65, 0.58, 0.02, 0.04, 0.01, 1.0, 1.0, 1.0)); // Sparkle

    // Friendly Big Smile
    setHijo(createPart(new CuboObj3D(), 0.0, 1.3, 0.54, 0.25, 0.04, 0.02, 0.95, 0.2, 0.2));

    // -------------------------------------------------------------------------
    // MUSHROOM HAT (The iconic giant hat)
    // -------------------------------------------------------------------------
    // Main White Cap (Massive)
    setHijo(createPart(new EsferaObj3D(), 0.0, 2.3, 0.0, 1.6, 1.1, 1.6, 1.0, 1.0, 1.0));
    
    // Red Spots (Varied and large)
    setHijo(createPart(new EsferaObj3D(),  0.0,  3.3,  0.0,  0.5,  0.2,  0.5,  1.0, 0.1, 0.1)); // Top
    setHijo(createPart(new EsferaObj3D(),  1.15, 2.4,  0.0,  0.4,  0.4,  0.4,  1.0, 0.1, 0.1)); // Right
    setHijo(createPart(new EsferaObj3D(), -1.15, 2.4,  0.0,  0.4,  0.4,  0.4,  1.0, 0.1, 0.1)); // Left
    setHijo(createPart(new EsferaObj3D(),  0.0,  2.4,  1.15, 0.4,  0.4,  0.4,  1.0, 0.1, 0.1)); // Front
    setHijo(createPart(new EsferaObj3D(),  0.0,  2.4, -1.15, 0.4,  0.4,  0.4,  1.0, 0.1, 0.1)); // Back

    // Extra Diagonal Spots
    setHijo(createPart(new EsferaObj3D(),  0.8,  2.7,  0.8,  0.35, 0.35, 0.35, 1.0, 0.1, 0.1)); // Front-Right
    setHijo(createPart(new EsferaObj3D(), -0.8,  2.7,  0.8,  0.35, 0.35, 0.35, 1.0, 0.1, 0.1)); // Front-Left
    setHijo(createPart(new EsferaObj3D(),  0.8,  2.7, -0.8,  0.35, 0.35, 0.35, 1.0, 0.1, 0.1)); // Back-Right
    setHijo(createPart(new EsferaObj3D(), -0.8,  2.7, -0.8,  0.35, 0.35, 0.35, 1.0, 0.1, 0.1)); // Back-Left

    // Lower Extra Spots
    setHijo(createPart(new EsferaObj3D(),  1.3,  1.9,  0.0,  0.3,  0.3,  0.3,  1.0, 0.1, 0.1)); // Right-Lower
    setHijo(createPart(new EsferaObj3D(), -1.3,  1.9,  0.0,  0.3,  0.3,  0.3,  1.0, 0.1, 0.1)); // Left-Lower
    setHijo(createPart(new EsferaObj3D(),  0.0,  1.9,  1.3,  0.3,  0.3,  0.3,  1.0, 0.1, 0.1)); // Front-Lower
    setHijo(createPart(new EsferaObj3D(),  0.0,  1.9, -1.3,  0.3,  0.3,  0.3,  1.0, 0.1, 0.1)); // Back-Lower

    // Shoes (Brown)
    setHijo(createPart(new CuboObj3D(), -0.3, 0.1, 0.0, 0.5, 0.3, 0.7, 0.45, 0.3, 0.15));
    setHijo(createPart(new CuboObj3D(),  0.3, 0.1, 0.0, 0.5, 0.3, 0.7, 0.45, 0.3, 0.15));

    setTranformacion(_vertex4<float>(Escalado_, 1.15, 1.15, 1.15));
}

Toad3D::~Toad3D() {
}
