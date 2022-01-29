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

#include "../Json.h"
#include "../Paint.h"
#include "../Theme.h"
#include "Button.h"
#include "Checkbox.h"
#include "Container.h"
#include "HorizontalContainer.h"
#include "Image.h"
#include "Panel.h"
#include "Separator.h"
#include "Text.h"
#include "TextInput.h"
#include "TextSelectable.h"
#include "VerticalContainer.h"

#include <algorithm>

namespace OctaneUI
{

Container::Container(Window* InWindow)
	: Control(InWindow)
	, m_UpdateLayout(false)
{
}

Container::~Container()
{
	m_Controls.clear();
}

std::shared_ptr<Text> Container::AddText(const char* Label)
{
	std::shared_ptr<Text> Result = AddControl<Text>(GetWindow());
	Result->SetText(Label);
	return Result;
}

std::shared_ptr<TextInput> Container::AddTextInput()
{
	std::shared_ptr<TextInput> Result = AddControl<TextInput>(GetWindow());
	return Result;
}

std::shared_ptr<TextSelectable> Container::AddTextSelectable(const char* Label)
{
	std::shared_ptr<TextSelectable> Result = AddControl<TextSelectable>(GetWindow());
	Result->SetText(Label);
	return Result;
}

std::shared_ptr<Button> Container::AddButton(const char* Label)
{
	std::shared_ptr<Button> Result = AddControl<Button>(GetWindow());
	Result->SetLabel(Label);
	return Result;
}

std::shared_ptr<Checkbox> Container::AddCheckbox(const char* Label)
{
	std::shared_ptr<Checkbox> Result = AddControl<Checkbox>(GetWindow());
	Result->SetLabel(Label);
	return Result;
}

std::shared_ptr<Image> Container::AddImage(const char* Path)
{
	std::shared_ptr<Image> Result = AddControl<Image>(GetWindow());
	Result->SetTexture(Path);
	return Result;
}

std::shared_ptr<Panel> Container::AddPanel()
{
	return AddControl<Panel>(GetWindow());
}

bool Container::ShouldUpdateLayout() const
{
	return m_UpdateLayout;
}

std::shared_ptr<Container> Container::AddContainer()
{
	return AddControl<Container>(GetWindow());
}

std::shared_ptr<HorizontalContainer> Container::AddHorizontalContainer()
{
	return AddControl<HorizontalContainer>(GetWindow());
}

std::shared_ptr<VerticalContainer> Container::AddVerticalContainer()
{
	return AddControl<VerticalContainer>(GetWindow());
}

std::shared_ptr<Control> Container::CreateControl(const std::string& Type)
{
	std::shared_ptr<Control> Result;

	if (Type == "Text")
	{
		Result = AddControl<Text>(GetWindow());
	}
	else if (Type == "TextSelectable")
	{
		Result = AddControl<TextSelectable>(GetWindow());
	}
	else if (Type == "TextInput")
	{
		Result = AddControl<TextInput>(GetWindow());
	}
	else if (Type == "Button")
	{
		Result = AddControl<Button>(GetWindow());
	}
	else if (Type == "Checkbox")
	{
		Result = AddControl<Checkbox>(GetWindow());
	}
	else if (Type == "Image")
	{
		Result = AddControl<Image>(GetWindow());
	}
	else if (Type == "Panel")
	{
		Result = AddControl<Panel>(GetWindow());
	}
	else if (Type == "Separator")
	{
		Result = AddControl<Separator>(GetWindow());
	}
	else if (Type == "HorizontalContainer")
	{
		Result = AddControl<HorizontalContainer>(GetWindow());
	}
	else if (Type == "VerticalContainer")
	{
		Result = AddControl<VerticalContainer>(GetWindow());
	}

	return Result;
}

Container* Container::InsertControl(const std::shared_ptr<Control>& Item, int Position)
{
	if (HasControl(Item))
	{
		return this;
	}

	Item->SetParent(this);
	Item->SetOnInvalidate([=](Control* Focus, InvalidateType Type)
	{
		OnInvalidate(Focus, Type);
	});

	if (Position >= 0)
	{
		m_Controls.insert(m_Controls.begin() + Position, Item);
	}
	else
	{
		m_Controls.push_back(Item);
	}

	OnInvalidate(Item.get(), InvalidateType::Both);

	return this;
}

Container* Container::RemoveControl(const std::shared_ptr<Control>& Item)
{
	auto Iter = std::find(m_Controls.begin(), m_Controls.end(), Item);
	if (Iter != m_Controls.end())
	{
		m_Controls.erase(Iter);
		Invalidate();
	}

	return this;
}

bool Container::HasControl(const std::shared_ptr<Control>& Item) const
{
	auto Iter = std::find(m_Controls.begin(), m_Controls.end(), Item);
	return Iter != m_Controls.end();
}

void Container::ClearControls()
{
	m_Controls.clear();
	m_UpdateLayout = true;
	Invalidate(InvalidateType::Both);
}

Container* Container::Layout()
{
	if (m_UpdateLayout)
	{
		Vector2 Size = CalculateSize(m_Controls);
		SetSize(Size);
		PlaceControls(m_Controls);

		for (const std::shared_ptr<Control>& Item : m_Controls)
		{
			const std::shared_ptr<Container> Child = std::dynamic_pointer_cast<Container>(Item);
			if (Child)
			{
				Child->Layout();
			}
		}

		m_UpdateLayout = false;
	}

	return this;
}

std::weak_ptr<Control> Container::GetControl(const Vector2& Point) const
{
	std::weak_ptr<Control> Result;

	for (int I = (int)m_Controls.size() - 1; I >= 0; I--)
	{
		const std::shared_ptr<Control>& Item = m_Controls[I];

		const std::shared_ptr<Container> ItemContainer = std::dynamic_pointer_cast<Container>(Item);
		if (ItemContainer)
		{
			Result = ItemContainer->GetControl(Point);
		}
		else if (Item->Contains(Point))
		{
			Result = Item;
		}

		if (!Result.expired())
		{
			break;
		}
	}

	return Result;
}

void Container::GetControls(std::vector<std::shared_ptr<Control>>& Controls) const
{
	for (const std::shared_ptr<Control>& Item : m_Controls)
	{
		const std::shared_ptr<Container>& ItemContainer = std::dynamic_pointer_cast<Container>(Item);
		if (ItemContainer)
		{
			ItemContainer->GetControls(Controls);
		}
		else
		{
			Controls.push_back(Item);
		}
	}
}

const char* Container::GetType() const
{
	return "Container";
}

void Container::OnPaint(Paint& Brush) const
{
	for (const std::shared_ptr<Control>& Item : m_Controls)
	{
		Item->OnPaint(Brush);
	}
}

void Container::OnLoad(const Json& Root)
{
	Control::OnLoad(Root);

	const Json& Controls = Root["Controls"];

	for (int I = 0; I < Controls.GetCount(); I++)
	{
		const Json& Item = Controls[I];

		std::string Type = Item["Type"].GetString();
		std::shared_ptr<Control> NewControl = CreateControl(Type);
		if (NewControl)
		{
			NewControl->OnLoad(Item);
		}
	}
}

bool Container::ExpandSize(Vector2& Size) const
{
	Control* Parent = GetParent();
	if (Parent == nullptr)
	{
		Size = GetSize();
		return false;
	}

	Expand Direction = GetExpand();
	switch (Direction)
	{
	case Expand::Both: Size = Parent->GetSize(); break;
	case Expand::Width: Size.X = Parent->GetSize().X; break;
	case Expand::Height: Size.Y = Parent->GetSize().Y; break;
	default: break;
	}

	return true;
}

Vector2 Container::GetPotentialSize(int& ExpandedControls) const
{
	Vector2 Result = GetSize();

	ExpandedControls = 0;
	for (const std::shared_ptr<Control>& Item : m_Controls)
	{
		const Vector2 Size = Item->GetSize();

		switch (Item->GetExpand())
		{
		case Expand::Both:
		case Expand::Width:
		case Expand::Height: ExpandedControls++; break;
		case Expand::None:
		default: Result -= Size; break;
		}
	}

	ExpandedControls = std::max<int>(ExpandedControls, 1);

	return Result;
}

Vector2 Container::CalculateSize(const std::vector<std::shared_ptr<Control>>& Controls) const
{
	Vector2 Result = GetSize();
	ExpandSize(Result);
	return Result;
}

void Container::PlaceControls(const std::vector<std::shared_ptr<Control>>& Controls) const
{
	for (const std::shared_ptr<Control>& Item : Controls)
	{
		Vector2 ItemSize = Item->GetSize();
		Expand Direction = Item->GetExpand();

		switch (Direction)
		{
		case Expand::Both: ItemSize = GetSize(); break;
		case Expand::Width: ItemSize.X = GetSize().X; break;
		case Expand::Height: ItemSize.Y = GetSize().Y; break;
		case Expand::None:
		default: break;
		}

		Item->SetSize(ItemSize);
	}

	for (const std::shared_ptr<Control>& Item : Controls)
	{
		Item->Update();
	}
}

void Container::OnInvalidate(Control* Focus, InvalidateType Type)
{
	m_UpdateLayout = Type == InvalidateType::Layout || Type == InvalidateType::Both || m_UpdateLayout;
	Invalidate(Type);
}

}
