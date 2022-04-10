/**

MIT License

Copyright (c) 2022 Mitchell Davis <mdavisprog@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "Tree.h"
#include "../Icons.h"
#include "../Json.h"
#include "../Paint.h"
#include "../ThemeProperties.h"
#include "../Window.h"
#include "Image.h"
#include "Text.h"
#include "VerticalContainer.h"

namespace OctaneGUI
{

#define TOGGLE_SIZE 16.0f

class TreeItem : public Control
{
	CLASS(TreeItem)

public:
	typedef std::function<void(bool)> OnHoveredSignature;

	TreeItem(Window* InWindow)
		: Control(InWindow)
	{
		m_Toggle = std::make_shared<Image>(InWindow);
		m_Text = std::make_shared<Text>(InWindow);

		m_Toggle
			->SetTexture(InWindow->GetIcons()->GetTexture())
			.SetParent(this);

		m_Text
			->SetParent(this)
			->SetPosition({ TOGGLE_SIZE, 0.0f });
	}

	TreeItem& SetText(const char* Text)
	{
		m_Text->SetText(Text);
		UpdateSize();
		Invalidate();
		return *this;
	}

	const char* GetText() const
	{
		return m_Text->GetText();
	}

	TreeItem& SetToggle(bool Expand)
	{
		const Icons::Type IconType = Expand ? Icons::Type::Expand : Icons::Type::Collapse;
		const Rect UVs = GetWindow()->GetIcons()->GetUVs(IconType);
		m_Toggle->SetUVs(UVs);
		m_Toggle->SetPosition({ TOGGLE_SIZE * 0.5f - UVs.GetSize().X * 0.5f, TOGGLE_SIZE * 0.5f - UVs.GetSize().Y * 0.5f });
		return *this;
	}

	TreeItem& SetHoveredColors()
	{
		const Variant& Color = GetProperty(ThemeProperties::TextSelectable_Text_Hovered);
		m_Text->SetProperty(ThemeProperties::Text, Color);
		m_Toggle->SetTint(Color.ToColor());
		return *this;
	}

	TreeItem& ClearHoveredColors()
	{
		m_Text->ClearProperty(ThemeProperties::Text);
		m_Toggle->SetTint(GetProperty(ThemeProperties::Check).ToColor());
		return *this;
	}

	TreeItem& SetOnToggle(OnEmptySignature&& Fn)
	{
		m_OnToggle = std::move(Fn);
		return *this;
	}

	TreeItem& SetOnHovered(OnHoveredSignature&& Fn)
	{
		m_OnHovered = std::move(Fn);
		return *this;
	}

	TreeItem& SetOnSelected(OnEmptySignature&& Fn)
	{
		m_OnSelected = std::move(Fn);
		return *this;
	}

	virtual void OnPaint(Paint& Brush) const override
	{
		m_Toggle->OnPaint(Brush);
		m_Text->OnPaint(Brush);
	}

	virtual bool OnMousePressed(const Vector2& Position, Mouse::Button Button) override
	{
		const Rect ToggleBounds = { GetAbsolutePosition(), GetAbsolutePosition() + Vector2(TOGGLE_SIZE, TOGGLE_SIZE) };
		if (ToggleBounds.Contains(Position))
		{
			if (m_OnToggle)
			{
				m_OnToggle();
			}
		}
		else
		{
			if (m_OnSelected)
			{
				m_OnSelected();
			}
		}

		return true;
	}

	virtual void OnMouseEnter() override
	{
		if (m_OnHovered)
		{
			m_OnHovered(true);
		}
	}

	virtual void OnMouseLeave() override
	{
		if (m_OnHovered)
		{
			m_OnHovered(false);
		}
	}

	virtual void OnThemeLoaded() override
	{
		Control::OnThemeLoaded();
		ClearHoveredColors();
	}

private:
	void UpdateSize()
	{
		Vector2 Size = { TOGGLE_SIZE, TOGGLE_SIZE };
		Size.X += m_Text->GetSize().X;
		Size.Y = std::max<float>(Size.Y, m_Text->GetSize().Y);
		SetSize(Size);
	}

	std::shared_ptr<Image> m_Toggle { nullptr };
	std::shared_ptr<Text> m_Text { nullptr };

	OnEmptySignature m_OnToggle { nullptr };
	OnHoveredSignature m_OnHovered { nullptr };
	OnEmptySignature m_OnSelected { nullptr };
};

Tree::Tree(Window* InWindow)
	: Container(InWindow)
{
	m_Item = AddControl<TreeItem>();
	m_Item
		->SetOnToggle([this]() -> void
			{
				SetExpand(!m_Expand);
			})
		.SetOnHovered([this](bool Hovered) -> void
			{
				if (m_OnHovered)
				{
					m_OnHovered(Hovered, m_Item);
				}
				else
				{
					SetHovered(Hovered, m_Item);
				}
			})
		.SetOnSelected([this]() -> void
			{
				if (m_OnSelected)
				{
					m_OnSelected(m_Item);
				}
				else
				{
					SetSelected(m_Item);
				}
			});
}

std::shared_ptr<Tree> Tree::AddChild(const char* Text)
{
	if (!m_List)
	{
		m_List = AddControl<VerticalContainer>();
		m_List
			->SetSpacing({ 0.0f, 0.0f })
			->SetPosition({ TOGGLE_SIZE + 4.0f, m_Item->GetSize().Y });
		SetExpand(true);
	}

	std::shared_ptr<Tree> Result = m_List->AddControl<Tree>();
	Result
		->SetText(Text)
		.SetOnHovered([this](bool Hovered, const std::shared_ptr<TreeItem>& Item) -> void
			{
				if (m_OnHovered)
				{
					m_OnHovered(Hovered, Item);
				}
				else
				{
					SetHovered(Hovered, Item);
				}
			})
		.SetOnSelected([this](const std::shared_ptr<TreeItem>& Item) -> void
			{
				if (m_OnSelected)
				{
					m_OnSelected(Item);
				}
				else
				{
					SetSelected(Item);
				}
			})
		.SetOnInvalidate([this](Control* Focus, InvalidateType Type) -> void
			{
				if (IsInLayout() && Type != InvalidateType::Paint)
				{
					return;
				}

				Invalidate(Focus, Type);
				InvalidateLayout();
			});

	InvalidateLayout();
	return Result;
}

Tree& Tree::SetText(const char* Text)
{
	m_Item->SetText(Text);
	return *this;
}

const char* Tree::GetText() const
{
	return m_Item->GetText();
}

Tree& Tree::SetExpand(bool Expand)
{
	if (m_Expand == Expand)
	{
		return *this;
	}

	if (!m_List)
	{
		return *this;
	}

	m_Expand = Expand;
	m_Item->SetToggle(m_Expand);

	if (m_List)
	{
		if (m_Expand)
		{
			if (!HasControl(m_List))
			{
				InsertControl(m_List);
			}
		}
		else
		{
			RemoveControl(m_List);
		}
	}

	return *this;
}

bool Tree::IsExpanded() const
{
	return m_Expand;
}

std::weak_ptr<Control> Tree::GetControl(const Vector2& Point) const
{
	return GetControl(Point, GetAbsoluteBounds());
}

Vector2 Tree::DesiredSize() const
{
	Vector2 Result = m_Item->GetSize();

	if (m_List && m_Expand)
	{
		const Vector2 Size = m_List->DesiredSize();
		Result.X = std::max<float>(Result.X, m_List->GetPosition().X + Size.X);
		Result.Y += Size.Y;
	}

	return Result;
}

void Tree::OnLoad(const Json& Root)
{
	Container::OnLoad(Root);

	SetText(Root["Text"].String());

	const Json& Items = Root["Items"];
	Items.ForEach([this](const Json& Item) -> void
		{
			std::shared_ptr<Tree> Child = AddChild("");
			Child->OnLoad(Item);
		});
}

void Tree::OnPaint(Paint& Brush) const
{
	std::shared_ptr<TreeItem> Hovered = m_Hovered.lock();
	std::shared_ptr<TreeItem> Selected = m_Selected.lock();

	if (Hovered && Hovered != Selected)
	{
		PaintSelection(Brush, Hovered);
	}

	if (Selected && !IsHidden(Selected))
	{
		PaintSelection(Brush, Selected);
	}

	Container::OnPaint(Brush);
}

Tree& Tree::SetOnHovered(OnHoveredTreeItemSignature&& Fn)
{
	m_OnHovered = std::move(Fn);
	return *this;
}

Tree& Tree::SetOnSelected(OnTreeItemSignature&& Fn)
{
	m_OnSelected = std::move(Fn);
	return *this;
}

std::weak_ptr<Control> Tree::GetControl(const Vector2& Point, const Rect& RootBounds) const
{
	const Vector2 Min = { RootBounds.Min.X, m_Item->GetAbsolutePosition().Y };
	const Vector2 Max = { RootBounds.Max.X, m_Item->GetAbsolutePosition().Y + m_Item->GetSize().Y };
	const Rect Bounds = { Min, Max };

	if (Bounds.Contains(Point))
	{
		return m_Item;
	}

	std::weak_ptr<Control> Result;
	if (!m_List || !m_Expand)
	{
		return Result;
	}

	for (const std::shared_ptr<Control>& Child : m_List->Controls())
	{
		const std::shared_ptr<Tree>& ChildTree = std::static_pointer_cast<Tree>(Child);
		Result = ChildTree->GetControl(Point, RootBounds);

		if (!Result.expired())
		{
			break;
		}
	}

	return Result;
}

void Tree::SetHovered(bool Hovered, const std::shared_ptr<TreeItem>& Item)
{
	std::shared_ptr<TreeItem> Hover = m_Hovered.lock();
	std::shared_ptr<TreeItem> Select = m_Selected.lock();

	if (Hover == Item)
	{
		if (!Hovered)
		{
			if (Select != Item)
			{
				Item->ClearHoveredColors();
			}
			m_Hovered.reset();
			Invalidate();
		}

		return;
	}

	if (Hovered)
	{
		Item->SetHoveredColors();
		m_Hovered = Item;
		Invalidate();
	}
}

void Tree::SetSelected(const std::shared_ptr<TreeItem>& Item)
{
	std::shared_ptr<TreeItem> Select = m_Selected.lock();

	if (Select == Item)
	{
		return;
	}

	if (Select)
	{
		Select->ClearHoveredColors();
	}

	m_Selected = Item;

	if (Item)
	{
		Item->SetHoveredColors();
	}
	
	Invalidate();
}

void Tree::PaintSelection(Paint& Brush, const std::shared_ptr<TreeItem>& Item) const
{
	const Vector2 Position = Item->GetAbsolutePosition();
	const Vector2 Min = { GetAbsolutePosition().X, Position.Y };
	const Vector2 Max = { GetAbsoluteBounds().Max.X, Position.Y + Item->GetSize().Y };
	Brush.Rectangle({ Min, Max }, GetProperty(ThemeProperties::TextSelectable_Hovered).ToColor());
}

bool Tree::IsHidden(const std::shared_ptr<TreeItem>& Item) const
{
	if (!m_List)
	{
		return false;
	}

	if (m_List->HasControl(Item))
	{
		return !m_Expand;
	}

	for (const std::shared_ptr<Control>& Child : m_List->Controls())
	{
		const std::shared_ptr<Tree>& ChildTree = std::static_pointer_cast<Tree>(Child);
		
		if (ChildTree->IsHidden(Item))
		{
			return true;
		}
	}

	return false;
}

}