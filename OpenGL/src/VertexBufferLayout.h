#pragma once

#include <vector>
#include <GL/glew.h>
#include "Renderer.h"

// This structure describes an element in a Vertex Buffer. 
// VertexBufferElement is used to describe an individual attribute in a vertex buffer (for example, a position or a color).
struct VertexBufferElement
{
	unsigned int count; // Number of components in the element (e.g. 3 for a 3D vector, 2 for a 2D vector, 4 for an RGBA color).
	unsigned int type; // Type of data
	unsigned char normalized; // Indicates whether data should be normalized when sent to the graphics card.
	
	// This method is used to determine the size in bytes of a given OpenGL type (for example, GL_FLOAT is 4 bytes).
	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT:			return 4;
		case GL_UNSIGNED_INT:	return 4;
		case GL_UNSIGNED_BYTE:	return 1;
		}
		ASSERT(false) // If the type does not correspond to a known case, the expression ASSERT(false)
		return 0;
	}

	// This constructor initializes the three members (count, type and normalized) when a VertexBufferElement instance is created.
	VertexBufferElement(unsigned int c, unsigned int t, unsigned char n) : count(c), type(t), normalized(n) {} 
};

// Designed to describe a layout (format or arrangement) of elements in a Vertex Buffer.
// VertexBufferLayout groups several VertexBufferElements to describe the complete structure of a vertex and calculates the total stride.
class VertexBufferLayout
{
private : 
	// A list of VertexBufferElement objects that describe the various attributes (such as positions, colors, etc.) of a vertex in the buffer.
	std::vector<VertexBufferElement> m_Elements; 

	// The total size (in bytes) of a vertex in the buffer.
	unsigned int m_Stride; 

public : 
	VertexBufferLayout()
		: m_Stride(0) {}

	// The Push method adds a new element to the layout, depending on its type(float, unsigned int, unsigned char).
	template<typename T>
	void Push(unsigned int count)
	{
		static_assert(false);
	}

	template<>
	void Push<float>(unsigned int count)
	{
		m_Elements.push_back({count, GL_FLOAT, GL_FALSE});
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<unsigned int>(unsigned int count)
	{
		m_Elements.push_back({ count, GL_UNSIGNED_INT, GL_FALSE});
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(unsigned int count)
	{
		m_Elements.push_back({ count, GL_UNSIGNED_BYTE,  GL_TRUE});
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	// Get attributs
	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; } // CONST&
	inline unsigned int GetStride() const { return m_Stride; }
};