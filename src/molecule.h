#include "atom.h"

typedef struct BondNode{
  unsigned int atom_index;
  unsigned int bond_type;
  struct BondNode *next;
} BondNode;

typedef struct {
  Atom* atoms;
  unsigned int atom_num;
  BondNode** adjacency_list;
} Molecule;

Molecule* create_molecule(Atom *atoms, unsigned int atom_num);
Molecule* molecule_from_file(const char* filepath);
void add_atom_to_molecule(Molecule *molecule, Atom *atom);
float* flatten_molecule_positions(const Molecule *molecule);
float* flatten_molecule_bonds(const Molecule *molecule, unsigned int* line_vertex_count);
void add_bond(Molecule *molecule, unsigned int atom1_index, unsigned int atom2_index, unsigned int bond_type);
void remove_bond(Molecule *molecule, unsigned int atom1_index, unsigned int atom2_index);
void print_adjacency_list(Molecule *molecule);
void delete_molecule(Molecule* mol);
