#include "Mesh.h"

//-----------------------------------------------------------------------------
// Name : Mesh (constructor)
//-----------------------------------------------------------------------------
// TODO: make vertices and indices movable to reduce overhead
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, QOpenGLShaderProgram * shader)
    :m_vertexBuffer(QOpenGLBuffer::VertexBuffer), m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{
    m_shader = shader;
    m_vertices = vertices;
    m_indices = indices;

    // Now that we have all the required data, set the vertex and indices buffers
    setupMesh();
}

//-----------------------------------------------------------------------------
// Name : Mesh (copy constructor)
//-----------------------------------------------------------------------------
Mesh::Mesh(const Mesh& copySubMesh)
    :m_vertices(copySubMesh.m_vertices),
     m_indices(copySubMesh.m_indices),
     m_vertexBuffer(QOpenGLBuffer::VertexBuffer),
     m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{
    m_shader = copySubMesh.m_shader;
    setupMesh();
}

//-----------------------------------------------------------------------------
// Name : Mesh (copy assignment)
//-----------------------------------------------------------------------------
Mesh& Mesh::operator=(const Mesh& copy)
{
    m_shader = copy.m_shader;
    m_vertices = copy.m_vertices;
    m_indices = copy.m_indices;

    setupMesh();

    return *this;
}

//-----------------------------------------------------------------------------
// Name : SubMesh (destructor)
//-----------------------------------------------------------------------------
Mesh::~Mesh()
{
}


//-----------------------------------------------------------------------------
// Name : Draw
//-----------------------------------------------------------------------------
void Mesh::Draw()
{
    QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    // Draw mesh
    m_vertexArrayObject.bind();
    f->glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
    m_vertexArrayObject.release();
}

//-----------------------------------------------------------------------------
// Name : setupMesh
//-----------------------------------------------------------------------------
void Mesh::setupMesh()
{
    // Make sure buffers were created
    if (!m_vertexArrayObject.isCreated())
        m_vertexArrayObject.create();

    if (!m_vertexBuffer.isCreated())
        m_vertexBuffer.create();

    if (!m_indexBuffer.isCreated())
        m_indexBuffer.create();

    // Load vertex data into vertex buffer
    m_vertexBuffer.bind();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_vertexBuffer.allocate(m_vertices.data(), m_vertices.size() * sizeof(Vertex));
    // Load indices data into index buffer
    m_indexBuffer.bind();
    m_indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_indexBuffer.allocate(m_indices.data(), m_indices.size() * sizeof(unsigned int));

    m_vertexBuffer.release();
    m_indexBuffer.release();

    // bind the vertex array object
    m_vertexArrayObject.bind();
    // bind the mesh buffers
    m_vertexBuffer.bind();
    m_indexBuffer.bind();
    // set how vertex data should be loaded to the shader
    m_shader->enableAttributeArray(0);
    m_shader->setAttributeBuffer(0, GL_FLOAT, 0, 3, sizeof(Vertex));
    m_shader->enableAttributeArray(1);
    m_shader->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, textureUV), 2, sizeof(Vertex));
    // realase the vertex array object
    m_vertexArrayObject.release();
}
