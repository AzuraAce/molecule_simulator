#include "atom.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static const char* element_symbols[] = {
    "",     // index 0 unused (atomic numbers start at 1)
    "H",  "He", "Li", "Be", "B",  "C",  "N",  "O",  "F",  "Ne",
    "Na", "Mg", "Al", "Si", "P",  "S",  "Cl", "Ar", "K",  "Ca",
    "Sc", "Ti", "V",  "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn",
    "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y",  "Zr",
    "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn",
    "Sb", "Te", "I",  "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd",
    "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb",
    "Lu", "Hf", "Ta", "W",  "Re", "Os", "Ir", "Pt", "Au", "Hg",
    "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th",
    "Pa", "U",  "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm",
    "Md", "No", "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds",
    "Rg", "Cn", "Nh", "Fl", "Mc", "Lv", "Ts", "Og"
};

Atom create_atom(int atomic_number, float mass, Vector3D pos, Vector3D vel){
  Atom new_atom = {atomic_number, element_symbols[atomic_number], mass, pos, vel};
  return new_atom;
}

float* flatten_position(const Atom* atom){
  float* flat_arr = (float*)malloc(sizeof(float) * 3);
  if(!flat_arr){
    fprintf(stderr, "Memory allocation failed in flatten_positon in atom.c\n");
    return NULL;
  }

  flat_arr[0] = atom->position.x;
  flat_arr[1] = atom->position.y;
  flat_arr[2] = atom->position.z;

  return flat_arr;
}

void update_position(Atom* atom, float time_step){
  atom->position.x += atom->velocity.x * time_step;
  atom->position.y += atom->velocity.y * time_step;
  atom->position.z += atom->velocity.z * time_step;

  return;
}

double get_kinetic_energy(Atom atom){
  double velocity_squared = (atom.velocity.x * atom.velocity.x) +
                           (atom.velocity.y * atom.velocity.y) +
                           (atom.velocity.z * atom.velocity.z);
  return 0.5 * atom.mass * velocity_squared;
}

double get_distance(Atom atom1, Atom atom2){
  return sqrt(
        ((atom2.position.x - atom1.position.x) * (atom2.position.x - atom1.position.x)) +
        ((atom2.position.y - atom1.position.y) * (atom2.position.y - atom1.position.y)) +
        ((atom2.position.z - atom1.position.z) * (atom2.position.z - atom1.position.z))
      );
}
