#include "modelos/luigi.hpp"

// Utility to create a colored part
static Objeto3D* createPart(Objeto3D* shape, float tx, float ty, float tz, float sx, float sy, float sz, float r, float g, float b) {
    shape->setTranformacion(_vertex4<float>(Translacion_, tx, ty, tz));
    shape->setTranformacion(_vertex4<float>(Escalado_, sx, sy, sz));
    shape->setColor(_vertex3<float>(r, g, b));
    shape->setAmb(_vertex4<float>(r * 0.4f, g * 0.4f, b * 0.4f, 1.0f));
    shape->setDiff(_vertex4<float>(r, g, b, 1.0f));
    return shape;
}

Luigi3D::Luigi3D(const std::string & filename) : Objeto3D() {
    // Body (Blue Overalls) - Taller
    setHijo(createPart(new CuboObj3D(), 0.0, 1.05, 0.0, 0.9, 1.4, 0.5, 0.1, 0.2, 0.8));

    // Buttons (Yellow)
    setHijo(createPart(new CuboObj3D(), -0.22, 1.5, 0.25, 0.12, 0.12, 0.1, 1.0, 0.9, 0.0));
    setHijo(createPart(new CuboObj3D(),  0.22, 1.5, 0.25, 0.12, 0.12, 0.1, 1.0, 0.9, 0.0));
    
    // Shirt (Green)
    setHijo(createPart(new CuboObj3D(), 0.0, 1.75, 0.0, 0.9, 0.4, 0.5, 0.1, 0.8, 0.1));

    // Head (Skin tone) - Slightly longer
    setHijo(createPart(new EsferaObj3D(), 0.0, 2.35, 0.0, 0.45, 0.6, 0.45, 1.0, 0.8, 0.6));
    
    // Nose (Skin tone, prominent and elongated)
    setHijo(createPart(new EsferaObj3D(), 0.0, 2.3, 0.55, 0.15, 0.2, 0.15, 1.0, 0.8, 0.6));

    // Mustache (Black/Brown) - Luigi has distinct halves
    setHijo(createPart(new CuboObj3D(),  0.18, 2.15, 0.35, 0.2, 0.1, 0.15, 0.1, 0.05, 0.02)); 
    setHijo(createPart(new CuboObj3D(), -0.18, 2.15, 0.35, 0.2, 0.1, 0.15, 0.1, 0.05, 0.02));

    // Eyes (White base + Blue iris + Black pupil)
    setHijo(createPart(new EsferaObj3D(), -0.12, 2.45, 0.4,  0.09, 0.12, 0.05, 1.0, 1.0, 1.0)); // Left White
    setHijo(createPart(new EsferaObj3D(), -0.12, 2.45, 0.45, 0.05, 0.07, 0.03, 0.1, 0.1, 0.8)); // Left Iris
    setHijo(createPart(new EsferaObj3D(), -0.12, 2.45, 0.48, 0.02, 0.03, 0.01, 0.0, 0.0, 0.0)); // Left Pupil

    setHijo(createPart(new EsferaObj3D(),  0.12, 2.45, 0.4,  0.09, 0.12, 0.05, 1.0, 1.0, 1.0)); // Right White
    setHijo(createPart(new EsferaObj3D(),  0.12, 2.45, 0.45, 0.05, 0.07, 0.03, 0.1, 0.1, 0.8)); // Right Iris
    setHijo(createPart(new EsferaObj3D(),  0.12, 2.45, 0.48, 0.02, 0.03, 0.01, 0.0, 0.0, 0.0)); // Right Pupil

    // Eyebrows (Black/Brown) - Luigi's are thinner/higher
    setHijo(createPart(new CuboObj3D(), -0.18, 2.65, 0.38, 0.18, 0.04, 0.1, 0.1, 0.05, 0.02)); // Left
    setHijo(createPart(new CuboObj3D(),  0.18, 2.65, 0.38, 0.18, 0.04, 0.1, 0.1, 0.05, 0.02)); // Right

    // Rounded Cap (Green) - Squashed Sphere
    setHijo(createPart(new EsferaObj3D(), 0.0, 2.78, 0.05, 0.45, 0.22, 0.5, 0.1, 0.8, 0.1));
    // Cap Visor
    setHijo(createPart(new CuboObj3D(), 0.0, 2.7, 0.45, 0.6, 0.05, 0.35, 0.1, 0.8, 0.1));

    // Cap Logo: White Circle + 'L'
    setHijo(createPart(new EsferaObj3D(), 0.0, 2.85, 0.45, 0.12, 0.12, 0.05, 1.0, 1.0, 1.0)); // White circle
    // Simplified 'L'
    setHijo(createPart(new CuboObj3D(), -0.02, 2.85, 0.5, 0.02, 0.08, 0.01, 0.0, 0.6, 0.0)); // L Vertical
    setHijo(createPart(new CuboObj3D(),  0.01, 2.82, 0.5, 0.06, 0.02, 0.01, 0.0, 0.6, 0.0)); // L Horizontal

    // Arms (Green)
    setHijo(createPart(new CuboObj3D(), -0.65, 1.65, 0.0, 0.35, 1.0, 0.35, 0.1, 0.8, 0.1)); // Left
    setHijo(createPart(new CuboObj3D(), 0.65, 1.65, 0.0, 0.35, 1.0, 0.35, 0.1, 0.8, 0.1));  // Right

    // Legs (Blue)
    setHijo(createPart(new CuboObj3D(), -0.3, 0.4, 0.0, 0.4, 1.0, 0.4, 0.1, 0.1, 0.7)); // Left
    setHijo(createPart(new CuboObj3D(), 0.3, 0.4, 0.0, 0.4, 1.0, 0.4, 0.1, 0.1, 0.7));  // Right

    // Shoes (Brown)
    setHijo(createPart(new CuboObj3D(), -0.3, 0.05, 0.1, 0.5, 0.2, 0.7, 0.3, 0.15, 0.05)); // Left
    setHijo(createPart(new CuboObj3D(), 0.3, 0.05, 0.1, 0.5, 0.2, 0.7, 0.3, 0.15, 0.05));  // Right

    setTranformacion(_vertex4<float>(Escalado_, 0.8, 0.8, 0.8));
}

Luigi3D::~Luigi3D() {
}
