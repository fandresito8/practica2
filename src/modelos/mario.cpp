#include "modelos/mario.hpp"

// Utility to create a colored part
static Objeto3D* createPart(Objeto3D* shape, float tx, float ty, float tz, float sx, float sy, float sz, float r, float g, float b) {
    shape->setTranformacion(_vertex4<float>(Translacion_, tx, ty, tz));
    shape->setTranformacion(_vertex4<float>(Escalado_, sx, sy, sz));
    shape->setColor(_vertex3<float>(r, g, b));
    shape->setAmb(_vertex4<float>(r * 0.4f, g * 0.4f, b * 0.4f, 1.0f));
    shape->setDiff(_vertex4<float>(r, g, b, 1.0f));
    return shape;
}

Mario3D::Mario3D(const std::string & filename) : Objeto3D() {
    // Body (Blue Overalls)
    setHijo(createPart(new CuboObj3D(), 0.0, 0.95, 0.0, 1.0, 1.2, 0.6, 0.1, 0.2, 0.8));
    
    // Buttons (Yellow)
    setHijo(createPart(new CuboObj3D(), -0.25, 1.3, 0.3, 0.15, 0.15, 0.1, 1.0, 0.9, 0.0));
    setHijo(createPart(new CuboObj3D(),  0.25, 1.3, 0.3, 0.15, 0.15, 0.1, 1.0, 0.9, 0.0));

    // Shirt (Red)
    setHijo(createPart(new CuboObj3D(), 0.0, 1.55, 0.0, 1.0, 0.4, 0.6, 1.0, 0.1, 0.1));

    // Head (Skin tone)
    setHijo(createPart(new EsferaObj3D(), 0.0, 2.15, 0.0, 0.5, 0.5, 0.5, 1.0, 0.8, 0.6));
    
    // Nose (Skin tone, prominent)
    setHijo(createPart(new EsferaObj3D(), 0.0, 2.15, 0.55, 0.18, 0.18, 0.18, 1.0, 0.8, 0.6));

    // Bushy Mustache (Black/Brown) - Multiple parts
    setHijo(createPart(new EsferaObj3D(),  0.0,  2.0,  0.45, 0.22, 0.12, 0.1, 0.15, 0.08, 0.05)); // Center
    setHijo(createPart(new EsferaObj3D(), -0.15, 1.95, 0.4,  0.18, 0.12, 0.1, 0.15, 0.08, 0.05)); // Left
    setHijo(createPart(new EsferaObj3D(),  0.15, 1.95, 0.4,  0.18, 0.12, 0.1, 0.15, 0.08, 0.05)); // Right

    // Eyes (White base + Blue iris + Black pupil)
    setHijo(createPart(new EsferaObj3D(), -0.15, 2.22, 0.45, 0.1, 0.12, 0.05, 1.0, 1.0, 1.0)); // Left White
    setHijo(createPart(new EsferaObj3D(), -0.15, 2.22, 0.5,  0.06, 0.08, 0.03, 0.1, 0.1, 0.8)); // Left Iris
    setHijo(createPart(new EsferaObj3D(), -0.15, 2.22, 0.53, 0.03, 0.04, 0.01, 0.0, 0.0, 0.0)); // Left Pupil

    setHijo(createPart(new EsferaObj3D(),  0.15, 2.22, 0.45, 0.1, 0.12, 0.05, 1.0, 1.0, 1.0)); // Right White
    setHijo(createPart(new EsferaObj3D(),  0.15, 2.22, 0.5,  0.06, 0.08, 0.03, 0.1, 0.1, 0.8)); // Right Iris
    setHijo(createPart(new EsferaObj3D(),  0.15, 2.22, 0.53, 0.03, 0.04, 0.01, 0.0, 0.0, 0.0)); // Right Pupil

    // Eyebrows (Black/Brown)
    setHijo(createPart(new CuboObj3D(), -0.22, 2.42, 0.4, 0.18, 0.06, 0.1, 0.1, 0.05, 0.02)); // Left
    setHijo(createPart(new CuboObj3D(),  0.22, 2.42, 0.4, 0.18, 0.06, 0.1, 0.1, 0.05, 0.02)); // Right

    // Rounded Cap (Red) - Squashed Sphere
    setHijo(createPart(new EsferaObj3D(), 0.0, 2.5, 0.05, 0.55, 0.25, 0.55, 1.0, 0.1, 0.1));
    // Cap Visor
    setHijo(createPart(new CuboObj3D(), 0.0, 2.4, 0.5, 0.7, 0.05, 0.45, 1.0, 0.1, 0.1));
    
    // Cap Logo: White Circle + 'M'
    setHijo(createPart(new EsferaObj3D(), 0.0, 2.58, 0.45, 0.15, 0.15, 0.05, 1.0, 1.0, 1.0)); // White circle
    // Simplified 'M' (two red vertical bars and a connector)
    setHijo(createPart(new CuboObj3D(), -0.05, 2.58, 0.5, 0.02, 0.08, 0.01, 0.8, 0.0, 0.0)); // M Left
    setHijo(createPart(new CuboObj3D(),  0.05, 2.58, 0.5, 0.02, 0.08, 0.01, 0.8, 0.0, 0.0)); // M Right
    setHijo(createPart(new CuboObj3D(),  0.0,  2.56, 0.5, 0.08, 0.02, 0.01, 0.8, 0.0, 0.0)); // M Bottom connector

    // Arms (Red)
    setHijo(createPart(new CuboObj3D(), -0.7, 1.45, 0.0, 0.4, 0.8, 0.4, 1.0, 0.1, 0.1)); // Left
    setHijo(createPart(new CuboObj3D(), 0.7, 1.45, 0.0, 0.4, 0.8, 0.4, 1.0, 0.1, 0.1));  // Right

    // Legs (Blue)
    setHijo(createPart(new CuboObj3D(), -0.3, 0.4, 0.0, 0.4, 0.8, 0.4, 0.1, 0.1, 0.7)); // Left
    setHijo(createPart(new CuboObj3D(), 0.3, 0.4, 0.0, 0.4, 0.8, 0.4, 0.1, 0.1, 0.7));  // Right

    // Shoes (Brown)
    setHijo(createPart(new CuboObj3D(), -0.3, 0.05, 0.1, 0.5, 0.2, 0.6, 0.4, 0.2, 0.1)); // Left
    setHijo(createPart(new CuboObj3D(), 0.3, 0.05, 0.1, 0.5, 0.2, 0.6, 0.4, 0.2, 0.1));  // Right
    
    // Final Scale
    setTranformacion(_vertex4<float>(Escalado_, 0.8, 0.8, 0.8));
}

Mario3D::~Mario3D() {
}
