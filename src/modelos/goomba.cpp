#include "modelos/goomba.hpp"

// Utility to create a colored part
static Objeto3D* createPart(Objeto3D* shape, float tx, float ty, float tz, float sx, float sy, float sz, float r, float g, float b) {
    shape->setTranformacion(_vertex4<float>(Translacion_, tx, ty, tz));
    shape->setTranformacion(_vertex4<float>(Escalado_, sx, sy, sz));
    shape->setColor(_vertex3<float>(r, g, b));
    shape->setAmb(_vertex4<float>(r * 0.4f, g * 0.4f, b * 0.4f, 1.0f));
    shape->setDiff(_vertex4<float>(r, g, b, 1.0f));
    return shape;
}

Goomba3D::Goomba3D(const std::string & filename) : Objeto3D() {
    // -------------------------------------------------------------------------
    // HEAD (The "Mushroom" / Triangular Shape)
    // -------------------------------------------------------------------------
    // Part 1: Top (Squashed sphere)
    setHijo(createPart(new EsferaObj3D(), 0.0, 1.3, 0.0, 1.0, 0.6, 1.0, 0.55, 0.35, 0.1));
    // Part 2: Middle (Wider flare)
    setHijo(createPart(new EsferaObj3D(), 0.0, 1.0, 0.0, 1.2, 0.4, 1.2, 0.55, 0.35, 0.1));
    // Part 3: Bottom Brim (The classic Goomba look)
    setHijo(createPart(new EsferaObj3D(), 0.0, 0.8, 0.0, 0.9, 0.2, 0.9, 0.5, 0.3, 0.05));

    // -------------------------------------------------------------------------
    // FACE
    // -------------------------------------------------------------------------
    // MASSIVE EYES (Iconic for Goombas)
    // White bases - Slightly angled inwards
    setHijo(createPart(new EsferaObj3D(), -0.25, 1.1, 0.8,  0.25, 0.35, 0.12, 1.0, 1.0, 1.0));
    setHijo(createPart(new EsferaObj3D(),  0.25, 1.1, 0.8,  0.25, 0.35, 0.12, 1.0, 1.0, 1.0));
    
    // Massive Black Pupils
    setHijo(createPart(new EsferaObj3D(), -0.25, 1.1, 0.9,  0.12, 0.22, 0.06, 0.0, 0.0, 0.0));
    setHijo(createPart(new EsferaObj3D(),  0.25, 1.1, 0.9,  0.12, 0.22, 0.06, 0.0, 0.0, 0.0));
    
    // Eye Sparkles
    setHijo(createPart(new EsferaObj3D(), -0.22, 1.18, 0.96, 0.03, 0.05, 0.01, 1.0, 1.0, 1.0));
    setHijo(createPart(new EsferaObj3D(),  0.28, 1.18, 0.96, 0.03, 0.05, 0.01, 1.0, 1.0, 1.0));

    // ANGRY EYEBROWS (Very thick and dark)
    Objeto3D* browL = createPart(new CuboObj3D(), -0.3, 1.48, 0.75, 0.5, 0.18, 0.15, 0.05, 0.02, 0.0);
    browL->setTranformacion(_vertex4<float>(Rotacion_, 2.0f, -25.0f, 0.0f)); 
    setHijo(browL);

    Objeto3D* browR = createPart(new CuboObj3D(),  0.3, 1.48, 0.75, 0.5, 0.18, 0.15, 0.05, 0.02, 0.0);
    browR->setTranformacion(_vertex4<float>(Rotacion_, 2.0f, 25.0f, 0.0f)); 
    setHijo(browR);

    // MOUTH & UNDERBITE TEETH
    // Mouth base (dark crevice)
    setHijo(createPart(new CuboObj3D(), 0.0, 0.65, 0.75, 0.4, 0.02, 0.02, 0.1, 0.05, 0.02));
    
    // Teeth (Two white squares pointing up)
    setHijo(createPart(new CuboObj3D(), -0.18, 0.7, 0.78, 0.12, 0.2, 0.1, 1.0, 1.0, 1.0));
    setHijo(createPart(new CuboObj3D(),  0.18, 0.7, 0.78, 0.12, 0.2, 0.1, 1.0, 1.0, 1.0));

    // -------------------------------------------------------------------------
    // BODY & FEET
    // -------------------------------------------------------------------------
    // Body (Pale Tan Cylinder-like)
    setHijo(createPart(new CuboObj3D(), 0.0, 0.4, 0.0, 0.7, 0.8, 0.7, 0.95, 0.85, 0.7));
    
    // Large Dark Feet ( karakteristik Goomba )
    setHijo(createPart(new EsferaObj3D(), -0.45, 0.2, 0.15, 0.45, 0.25, 0.55, 0.15, 0.1, 0.05));
    setHijo(createPart(new EsferaObj3D(),  0.45, 0.2, 0.15, 0.45, 0.25, 0.55, 0.15, 0.1, 0.05));

    setTranformacion(_vertex4<float>(Escalado_, 1.3, 1.3, 1.3));
}

Goomba3D::~Goomba3D() {
}
