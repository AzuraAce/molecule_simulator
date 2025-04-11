#ifndef ATOM_H
#define ATOM_H

typedef struct {
  float x;
  float y;
  float z;
} Vector3D;

typedef struct {
  int atomic_number;
  const char* element_symbol;
  double mass;
  Vector3D position;
  Vector3D velocity;
} Atom;

Atom create_atom(int atomic_number, float mass, Vector3D pos, Vector3D vel);
float* flatten_position(const Atom* atom);
void update_position(Atom* atom, float time_step);
double get_kinetic_energy(Atom atom);
double get_distance(Atom atom1, Atom atom2);

#endif // !ATOM_H
