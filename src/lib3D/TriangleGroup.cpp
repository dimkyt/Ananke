#include "lib3d\TriangleGroup.h"

TriangleGroup::TriangleGroup()
  : m_has_texture(false),
    m_has_normal(false),
    m_num_faces(0)
{
}

TriangleGroup::~TriangleGroup()
{
}

void TriangleGroup::add_vertex_index(unsigned int index) {
  m_vertex_index.push_back(index);
}

void TriangleGroup::add_texture_index(unsigned int index) {
  m_texture_index.push_back(index);
}

void TriangleGroup::add_normal_index(unsigned int index) {
  m_normal_index.push_back(index);
}

void TriangleGroup::add_face() {
  m_num_faces++;
}

void TriangleGroup::set_material_index(unsigned int index) {
  m_material_index = index;
}

void TriangleGroup::set_texture_flag(bool has_texture) {
  m_has_texture = has_texture;
}

void TriangleGroup::set_normal_flag(bool has_normal) {
  m_has_normal = has_normal;
}

unsigned int TriangleGroup::get_vertex_index(unsigned int index) const {
  return m_vertex_index[index];
}

unsigned int TriangleGroup::get_texture_index(unsigned int index) const {
  return m_texture_index[index];
}

unsigned int TriangleGroup::get_normal_index(unsigned int index) const {
  return m_normal_index[index];
}

unsigned int TriangleGroup::get_num_vertex_index() const {
  return static_cast<unsigned int>(m_vertex_index.size());
}

unsigned int TriangleGroup::get_num_texture_index() const {
  return static_cast<unsigned int>(m_texture_index.size());
}

unsigned int TriangleGroup::get_num_normal_index() const {
  return static_cast<unsigned int>(m_normal_index.size());
}

unsigned int TriangleGroup::get_num_faces() const {
  return m_num_faces;
}

unsigned int TriangleGroup::get_material_index() const {
  return m_material_index;
}

void TriangleGroup::trim_vectors() {
  vector<unsigned int>(m_vertex_index).swap(m_vertex_index);
  vector<unsigned int>(m_texture_index).swap(m_texture_index);
  vector<unsigned int>(m_normal_index).swap(m_normal_index);
}