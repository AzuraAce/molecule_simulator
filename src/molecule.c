#include "molecule.h"
#include <stdlib.h>
#include <stdio.h>


Molecule* init_molecule(){
  Molecule* molecule = (Molecule*)malloc(sizeof(Molecule));

  molecule->atom_num = 0;

  molecule->atoms = (Atom*)malloc(sizeof(Atom));
  if(molecule->atoms == NULL){
    fprintf(stderr, "Error: Could not allocate memory for molecule->atoms\n");
    free(molecule);
    return NULL;
  }
  molecule->adjacency_list = (BondNode**)malloc(sizeof(BondNode*));
  if(molecule->adjacency_list == NULL){
    fprintf(stderr, "Error: Could not allocate memory for molecule->adjacency_list\n");
    free(molecule->atoms);
    free(molecule);
    return NULL;
  }

  return molecule;
}

Molecule* create_molecule(Atom *atoms, unsigned int atom_num){
  if(atoms == NULL || atom_num == 0){
    fprintf(stderr, "Error: Cannot create molecule with NULL atoms\n");
    return NULL;
  }
  
  Molecule* molecule = (Molecule*)malloc(sizeof(Molecule));

  molecule->atoms = (Atom*)malloc(sizeof(Atom) * atom_num);
  if(molecule->atoms == NULL){
    fprintf(stderr, "Error: Could not allocate memory for molecule->atoms\n");
    free(molecule);
    return NULL;
  }
  molecule->adjacency_list = (BondNode**)malloc(sizeof(BondNode*) * atom_num);
  if(molecule->adjacency_list == NULL){
    fprintf(stderr, "Error: Could not allocate memory for molecule->adjacency_list\n");
    free(molecule->atoms);
    free(molecule);
    return NULL;
  }

  molecule->atom_num = atom_num;
  for(unsigned int i = 0; i < atom_num; i++){
    molecule->atoms[i] = atoms[i];
    molecule->adjacency_list[i] = NULL;
  }

  return molecule;
}

void add_atom_to_molecule(Molecule *molecule, Atom *atom){
  if(molecule == NULL){
    fprintf(stderr, "Error: Cannot add atom to NULL molecule\n");
    return;
  }

  Atom* atoms_new = (Atom*)realloc(molecule->atoms, sizeof(Atom) * (molecule->atom_num + 1));
  if(atoms_new == NULL){
    fprintf(stderr, "Error: Could not reallocate atoms array\n");
    return;
  }

  molecule->atoms = atoms_new;
  molecule->atoms[molecule->atom_num] = *atom;

  // Reallocate adjacency list
  BondNode** new_adj = realloc(molecule->adjacency_list, (molecule->atom_num + 1) * sizeof(BondNode*));
  if (!new_adj) return;

  molecule->adjacency_list = new_adj;
  molecule->adjacency_list[molecule->atom_num] = NULL;  // new atom has no bonds yet 

  molecule->atom_num++;

  return;
}

float* flatten_molecule_positions(const Molecule* molecule){
  float* flat_arr = (float*)malloc(sizeof(float) * molecule->atom_num * 3);
  if(!flat_arr){
    fprintf(stderr, "Memory allocation failed in flatten_molecule_positons in molecule.c\n");
    return NULL;
  }

  for (unsigned int i = 0; i < molecule->atom_num; i++) {
    float* flat_atom_arr = flatten_position(&molecule->atoms[i]);
    flat_arr[(i * 3)] = flat_atom_arr[0];
    flat_arr[(i * 3) + 1] = flat_atom_arr[1];
    flat_arr[(i * 3) + 2] = flat_atom_arr[2];

    free(flat_atom_arr);
  }

  return flat_arr;
}

float* flatten_molecule_bonds(const Molecule *molecule, unsigned int* line_vertex_count){
  float* lines = (float*)malloc(sizeof(float) * (molecule->atom_num * 2) * 3);
  unsigned int bond_index = 0;

  for (unsigned int atom1_index = 0; atom1_index < molecule->atom_num; atom1_index++) {
    Vector3D atom1_pos = molecule->atoms[atom1_index].position;

    for (BondNode* bond = molecule->adjacency_list[atom1_index]; bond != NULL; bond = bond->next) {
      unsigned int atom2_index = bond->atom_index;

      if(atom1_index < atom2_index){
        Vector3D atom2_pos = molecule->atoms[atom2_index].position;

        lines[bond_index++] = atom1_pos.x;
        lines[bond_index++] = atom1_pos.y;
        lines[bond_index++] = atom1_pos.z;

        lines[bond_index++] = atom2_pos.x;
        lines[bond_index++] = atom2_pos.y;
        lines[bond_index++] = atom2_pos.z;
      }
    }
  }

  *line_vertex_count = (bond_index + 1) / 3;
  return lines;
}

void add_bond(Molecule *molecule, unsigned int atom1_index, unsigned int atom2_index, unsigned bond_type){
  if(atom1_index < molecule->atom_num && atom2_index < molecule->atom_num) {
    // Add atom2 to atom1's adjacency_list
    BondNode *new_node1 = (BondNode*)malloc(sizeof(BondNode));
    new_node1->atom_index = atom2_index;
    new_node1->bond_type = bond_type;
    new_node1->next = molecule->adjacency_list[atom1_index];
    molecule->adjacency_list[atom1_index] = new_node1;

    // Add atom1 to atom2's adjacency_list
    BondNode *new_node2 = (BondNode*)malloc(sizeof(BondNode));
    new_node2->atom_index = atom1_index;
    new_node2->bond_type = bond_type;
    new_node2->next = molecule->adjacency_list[atom2_index];
    molecule->adjacency_list[atom2_index] = new_node2;
  } else {
    printf("Invalid Atom Index.\n");
  }
}

void remove_bond(Molecule *molecule, unsigned int atom1_index, unsigned int atom2_index){
  if(atom1_index < molecule->atom_num && atom2_index < molecule->atom_num){
    // remove atom2 from atom1's adjacency_list
    BondNode** current = &(molecule->adjacency_list[atom1_index]);
    while (*current) {
        if ((*current)->atom_index == atom2_index) {
            BondNode* to_delete = *current;
            *current = (*current)->next;
            free(to_delete);
            break;
        }
        current = &((*current)->next);
    }
    // remove atom1 from atom2's adjacency_list
    current = &(molecule->adjacency_list[atom2_index]);
    while (*current) {
        if ((*current)->atom_index == atom1_index) {
            BondNode* to_delete = *current;
            *current = (*current)->next;
            free(to_delete);
            break;
        }
        current = &((*current)->next);
    }
  } else {
    printf("Invalid Atom Index.\n");
  }
}

Molecule* molecule_from_file(const char *filepath){
  FILE* file = fopen(filepath, "r");
  if(!file){
    fprintf(stderr, "Error: Failed to open file with path: %s\n", filepath);
    return NULL;
  }

  char line[256];
  int reading_atoms = 0, reading_bonds = 0;
  unsigned int atom_num, line_index = 0;

  Molecule* molecule = init_molecule();

  while(fgets(line, sizeof(line), file)){
    if(line_index >= atom_num){
      reading_atoms = 0;
      reading_bonds = 1;
    }

    if(line[0] == '#' || line[0] == '\n') continue;

    if(!reading_atoms && !reading_bonds){
      // read number of atoms
      sscanf(line, "%d", &atom_num);
      reading_atoms = 1;
      continue;
    }
  
    if (reading_atoms) {
        int atomic_number;
        char symbol[3];
        double mass;
        float x, y, z, vx, vy, vz;
        sscanf(line, "%d %2s %lf %f %f %f %f %f %f", &atomic_number, symbol, &mass, &x, &y, &z, &vx, &vy, &vz);
        Atom atom = create_atom(atomic_number, mass, (Vector3D){x, y, z}, (Vector3D){vx, vy, vz});
        add_atom_to_molecule(molecule, &atom);
        line_index++;
    } else if (reading_bonds) {
        int a1, a2, bond_type;
        sscanf(line, "%d %d %d", &a1, &a2, &bond_type);
        add_bond(molecule, a1, a2, bond_type);
    }
  }

  return molecule;
}

void print_adjacency_list(Molecule *molecule){
  printf("adjacency_list:\n");
  for(unsigned int i = 0; i < molecule->atom_num; i++){
    printf("Atom %s: ", molecule->atoms[i].element_symbol);
    BondNode *current = molecule->adjacency_list[i];
    while(current != NULL){
      printf("%s -> ", molecule->atoms[current->atom_index].element_symbol);
      current = current->next;
    }
    printf("NULL\n");
  }
}

void delete_molecule(Molecule *mol){
  for(int i = 0; i < mol->atom_num; i++){
    BondNode* current = mol->adjacency_list[i];
    
    while(current != NULL){
      BondNode* next = current->next;
      free(current);
      current = next;
    }
  }

  free(mol->adjacency_list);
  free(mol->atoms);
  free(mol);
}
