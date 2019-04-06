#include "GameObject.h"

GameObject::GameObject()
{
	SetName("unnamed");
}

GameObject::GameObject(std::string m_name)
{
	SetName(m_name);
}

GameObject::~GameObject()
{
}

void GameObject::Translate(Vector2 delta)
{
	Position += delta;
}

std::string GameObject::GetName()
{
	return m_name;
}

void GameObject::SetName(std::string name)
{
	if (name.length() == 0) m_name = "unnamed";
	else m_name = name;
}

void GameObject::SetRadius(int32_t radius) {
	m_rendProps.radius = (radius <= 1) ? 6 : radius;
}

void GameObject::SetColor(olc::Pixel color) {
	m_rendProps.color = color;
}

void GameObject::SetRenderProperties(RenderProperties renderProperties)
{
	m_rendProps = renderProperties;
}

void GameObject::Render(olc::PixelGameEngine & engine, bool displayName) {
	engine.DrawCircle(Position.X(), Position.Y(), m_rendProps.radius, m_rendProps.color);
	float nameOffset = m_name.length() * 3.5;
	if (displayName) engine.DrawString(Position.X() - nameOffset, Position.Y() - m_rendProps.radius * 2 - 5, m_name, m_rendProps.color);
}

void GameObject::IterateColor()
{
	m_color_index++;
	if (m_color_index >= colors_count) m_color_index = 0;
	m_rendProps.color = object_colors[m_color_index];

}
