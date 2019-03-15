#ifndef MESH_H
#define MESH_H

#include <vector>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QVector3D>

struct Vertex
{
    Vertex(const QVector3D& _position, const QVector2D& _textureUV)
        :position(_position), textureUV(_textureUV)  {}
    Vertex(QVector3D&& _position, QVector2D&& _textureUV)
        :position(std::move(_position)),textureUV(std::move(_textureUV))  {}

    Vertex& operator=(const Vertex& copy)
    {
        position = copy.position;
        textureUV = copy.textureUV;

        return *this;
    }

    QVector3D position;
    QVector2D textureUV;
};

class Mesh
{
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, QOpenGLShaderProgram * shader);
    Mesh(const Mesh& copySubMesh);
    Mesh& operator=(const Mesh& copy);

    void Draw();

    ~Mesh();

private:
    void setupMesh();

    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    QOpenGLShaderProgram * m_shader;
    QOpenGLVertexArrayObject m_vertexArrayObject;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_indexBuffer;
};

#endif // MESH_H
