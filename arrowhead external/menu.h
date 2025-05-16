#pragma once
#include <string>
#include <vector>

class Menu
{
private:

	enum class ItemType
	{
		Useless,
		Boolean,
		List,
		Int,
		Float,
		Arrow
	};

	typedef struct BooleanItem
	{
		void* Data;
	};

	typedef struct ListItem
	{
		int* Index;
		void* Data;
	};

	typedef struct FloatItem
	{
		float Min;
		float Max;
		float Step;
		void* Data;
	};
	typedef struct ArrowItem
	{
		void* Data;
	};
	typedef struct IntItem
	{
		int Min;
		int Max;
		int Step;
		void* Data;
	};

	typedef struct MenuItem
	{
		std::string Name;
		ItemType Type;
		union
		{

			BooleanItem Boolean;
			ArrowItem Arrow;
			ListItem List;
			FloatItem Float;
			IntItem Int;

		} Data;
	};

	int Index = 0;
	std::vector<MenuItem> Items;

	std::string MenuTitle = "";
	std::string MenuFooter = "";

public:

	void CreateMenu(std::string Title, std::string Footer)
	{
		MenuTitle = Title;
		MenuFooter = Footer;
	}

	void AddBooleanItem(std::string Name, void* Data)
	{
		MenuItem Item;
		Item.Name = Name;
		Item.Type = ItemType::Boolean;

		BooleanItem Buffer;
		Buffer.Data = Data;

		Item.Data.Boolean = Buffer;

		Items.push_back(Item);
	}

	void AddListItem(std::string Name, void* Data, int* Index)
	{
		MenuItem Item;
		Item.Name = Name;
		Item.Type = ItemType::List;

		ListItem Buffer;
		Buffer.Index = Index;
		Buffer.Data = Data;

		Item.Data.List = Buffer;

		Items.push_back(Item);
	}
	void AddArrowItem(std::string Name, void* Data)
	{
		MenuItem Item;
		Item.Name = Name;
		Item.Type = ItemType::Arrow;

		ArrowItem Buffer;
		Buffer.Data = Data;

		Item.Data.Arrow = Buffer;

		Items.push_back(Item);
	}
	void AddIntItem(std::string Name, int Min, int Max, int Step, void* Data)
	{
		MenuItem Item;
		Item.Name = Name;
		Item.Type = ItemType::Int;

		IntItem Buffer;
		Buffer.Min = Min;
		Buffer.Max = Max;
		Buffer.Step = Step;
		Buffer.Data = Data;

		Item.Data.Int = Buffer;

		Items.push_back(Item);
	}

	void AddFloatItem(std::string Name, float Min, float Max, float Step, void* Data)
	{

		MenuItem Item;
		Item.Name = Name;
		Item.Type = ItemType::Float;

		FloatItem Buffer;
		Buffer.Min = Min;
		Buffer.Max = Max;
		Buffer.Step = Step;
		Buffer.Data = Data;

		Item.Data.Float = Buffer;

		Items.push_back(Item);
	}

	void AddUselessItem(std::string Name)
	{
		MenuItem Item;
		Item.Name = Name;
		Item.Type = ItemType::Useless;

		Items.push_back(Item);
	}

	void SetIndex(int X)
	{
		Index = X;
	}
	void Navigation()
	{
		if (GetAsyncKeyState(VK_DOWN) && 0x8000)
		{
			if (Index < Items.size() - 1 && Items.at(Index + 1).Type != ItemType::Useless)
				Index++;
			else if (Index < Items.size() - 3 && Items.at(Index + 1).Type == ItemType::Useless)
				Index += 2;

			Sleep(150);
		}

		if (GetAsyncKeyState(VK_UP) && 0x8000)
		{
			if (Index > 0 && Items.at(Index - 1).Type != ItemType::Useless)
				Index--;
			else if (Index >= 3 && Items.at(Index - 1).Type == ItemType::Useless)
				Index -= 2;

			Sleep(150);
		}

		if (GetAsyncKeyState(VK_RIGHT) && 0x8000)
		{
			switch (Items.at(Index).Type)
			{
			case ItemType::Boolean:
				*(bool*)(Items.at(Index).Data.Boolean.Data) = true;
				break;
			case ItemType::Arrow:
				*(bool*)(Items.at(Index).Data.Boolean.Data) = true;
				break;
			case ItemType::List:
				if ((*(Items.at(Index).Data.List.Index)) < ((std::vector<std::string>*)(Items.at(Index).Data.List.Data))->size() - 1)
					(*(Items.at(Index).Data.List.Index))++;
				break;

			case ItemType::Int:
				if (*(int*)Items.at(Index).Data.Int.Data + Items.at(Index).Data.Int.Step <= Items.at(Index).Data.Int.Max)
					*(int*)Items.at(Index).Data.Int.Data += Items.at(Index).Data.Int.Step;
				break;

			case ItemType::Float:
				if (*(float*)Items.at(Index).Data.Float.Data + Items.at(Index).Data.Float.Step <= Items.at(Index).Data.Float.Max)
					*(float*)Items.at(Index).Data.Float.Data += Items.at(Index).Data.Float.Step;
				break;

			default:
				break;
			}
			Sleep(150);
		}

		if (GetAsyncKeyState(VK_LEFT) && 0x8000)
		{
			switch (Items.at(Index).Type)
			{
			case ItemType::Boolean:
				*(bool*)(Items.at(Index).Data.Boolean.Data) = false;
				break;
			case ItemType::Arrow:
				*(bool*)(Items.at(Index).Data.Boolean.Data) = false;
				break;
			case ItemType::List:
				if (*(Items.at(Index).Data.List.Index) > 0)
					(*(Items.at(Index).Data.List.Index))--;
				break;

			case ItemType::Int:
				if (*(int*)Items.at(Index).Data.Int.Data - Items.at(Index).Data.Int.Step >= Items.at(Index).Data.Int.Min)
					*(int*)Items.at(Index).Data.Int.Data -= Items.at(Index).Data.Int.Step;
				break;

			case ItemType::Float:
				if (*(float*)Items.at(Index).Data.Float.Data - Items.at(Index).Data.Float.Step >= Items.at(Index).Data.Float.Min)
					*(float*)Items.at(Index).Data.Float.Data -= Items.at(Index).Data.Float.Step;
				break;

			default:
				break;
			}
			Sleep(150);
		}
	}


	void TextCentered(std::string text)
	{
		auto windowWidth = ImGui::GetWindowSize().x;
		auto textWidth = ImGui::CalcTextSize(text.c_str()).x;
		ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
		ImGui::TextColored(ImColor(255, 0, 0), text.c_str());
	}

	void MenuLoop()
	{


		TextCentered(xor ("[ARMA2OA Legacy]"));
		ImGui::NewLine();

		for (int I = 0; I < Items.size(); I++)
		{
			if (I == Index)
			{
				switch (Items.at(I).Type)
				{
				case ItemType::Boolean:
					//printf("> %s \t [%s] \n", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off" );
					//ImGui::TextColored(ImColor(0, 255, 0), "%s \t [%s]", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off");
					ImGui::TextColored(ImColor(0, 255, 0), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 150);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("[%s]"), *(bool*)(Items.at(I).Data.Boolean.Data) ? xor ("on") : xor ("off"));
					break;
				case ItemType::Arrow:
					//printf("> %s \t [%s] \n", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off" );
					//ImGui::TextColored(ImColor(0, 255, 0), "%s \t [%s]", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off");
					ImGui::TextColored(ImColor(0, 255, 0), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 150);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("[%s]"), *(bool*)(Items.at(I).Data.Boolean.Data) ? xor ("...") : xor ("-->"));
					break;

				case ItemType::List:
					//printf("> %s \t [%s] \n", Items.at(I).Name.c_str(), ((std::vector<std::string>*)(Items.at(I).Data.List.Data))->at(*(Items.at(I).Data.List.Index)).c_str());
					//ImGui::TextColored(ImColor(0, 255, 0), "%s \t [%s]", Items.at(I).Name.c_str(), ((std::vector<std::string>*)(Items.at(I).Data.List.Data))->at(*(Items.at(I).Data.List.Index)).c_str());
					ImGui::TextColored(ImColor(0, 255, 0), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 150);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("[%s]"), ((std::vector<std::string>*)(Items.at(I).Data.List.Data))->at(*(Items.at(I).Data.List.Index)).c_str());
					break;

				case ItemType::Int:
					//printf("> %s \t [%d] \n", Items.at(I).Name.c_str(), *(int*)Items.at(I).Data.Int.Data);
					//ImGui::TextColored(ImColor(0, 255, 0), "%s \t [%d]", Items.at(I).Name.c_str(), *(int*)Items.at(I).Data.Int.Data);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 150);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("[%d]"), *(int*)Items.at(I).Data.Int.Data);
					break;

				case ItemType::Float:
					//printf("> %s \t [%0.3f] \n", Items.at(I).Name.c_str(), *(float*)Items.at(I).Data.Float.Data);
					//ImGui::TextColored(ImColor(0, 255, 0), "%s \t [% 0.3f]", Items.at(I).Name.c_str(), *(float*)Items.at(I).Data.Float.Data);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 150);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("[%0.1f]"), *(float*)Items.at(I).Data.Float.Data);
					break;

				default:
					break;
				}
			}
			else
			{
				switch (Items.at(I).Type)
				{
				case ItemType::Boolean:
					//printf("  %s \t [%s] \n", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off");
					//ImGui::TextColored(ImColor(255, 0, 0), "%s \t [%s]", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off");
					ImGui::TextColored(ImColor(255, 255, 255), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 150);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("[%s]"), *(bool*)(Items.at(I).Data.Boolean.Data) ? xor ("on") : xor ("off"));
					break;
				case ItemType::Arrow:
					//printf("  %s \t [%s] \n", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off");
					//ImGui::TextColored(ImColor(255, 0, 0), "%s \t [%s]", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off");
					ImGui::TextColored(ImColor(255, 255, 255), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 150);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("[%s]"), *(bool*)(Items.at(I).Data.Boolean.Data) ? xor ("...") : xor ("-->"));
					break;

				case ItemType::List:
					//printf("  %s \t [%s] \n", Items.at(I).Name, ((std::vector<std::string>*)(Items.at(I).Data.List.Data))->at(*(Items.at(I).Data.List.Index)).c_str());
					//ImGui::TextColored(ImColor(255, 0, 0), "%s \t [%s]", Items.at(I).Name.c_str(), ((std::vector<std::string>*)(Items.at(I).Data.List.Data))->at(*(Items.at(I).Data.List.Index)).c_str());
					ImGui::TextColored(ImColor(255, 255, 255), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 150);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("[%s]"), ((std::vector<std::string>*)(Items.at(I).Data.List.Data))->at(*(Items.at(I).Data.List.Index)).c_str());
					break;

				case ItemType::Int:
					//printf("  %s \t [%d] \n", Items.at(I).Name.c_str(), *(int*)Items.at(I).Data.Int.Data);
					//ImGui::TextColored(ImColor(255, 0, 0), "%s \t [%d]", Items.at(I).Name.c_str(), *(int*)Items.at(I).Data.Int.Data);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 150);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("[%d]"), *(int*)Items.at(I).Data.Int.Data);
					break;

				case ItemType::Float:
					//printf("  %s \t [%0.3f] \n", Items.at(I).Name.c_str(), *(float*)Items.at(I).Data.Float.Data);
					//ImGui::TextColored(ImColor(255, 0, 0), "%s \t [% 0.3f]", Items.at(I).Name.c_str(), *(float*)Items.at(I).Data.Float.Data);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 150);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("[%0.1f]"), *(float*)Items.at(I).Data.Float.Data);
					break;

				case ItemType::Useless:
					ImGui::TextColored(ImColor(255, 255, 0), xor ("%s"), Items.at(I).Name.c_str());
					break;

				default:
					break;
				}
			}
		}

		ImGui::NewLine();
		TextCentered(xor ("[Open/Close - INSERT]"));
		//printf("%s \n", MenuFooter.c_str());
		//if (MenuFooter != "");
		//{
		//	ImGui::NewLine();
		//	ImGui::TextColored(ImColor(255, 255, 0), "%s", MenuFooter.c_str());
		//}

		//Sleep(100);
		//system("cls");
	}
	void ItemsLoop()
	{
		TextCentered(xor ("[Biboran]"));
		ImGui::NewLine();

		for (int I = 0; I < Items.size(); I++)
		{
			if (I == Index)
			{
				switch (Items.at(I).Type)
				{
				case ItemType::Boolean:
					//printf("> %s \t [%s] \n", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off" );
					//ImGui::TextColored(ImColor(0, 255, 0), "%s \t [%s]", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off");
					ImGui::TextColored(ImColor(0, 255, 0), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 5);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("[%s]"), *(bool*)(Items.at(I).Data.Boolean.Data) ? xor ("on") : xor ("off"));
					break;
				case ItemType::Arrow:
					//printf("> %s \t [%s] \n", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off" );
					//ImGui::TextColored(ImColor(0, 255, 0), "%s \t [%s]", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off");
					ImGui::TextColored(ImColor(0, 255, 0), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 5);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("[%s]"), *(bool*)(Items.at(I).Data.Boolean.Data) ? xor ("<--") : xor ("..."));
					break;

				case ItemType::List:
					//printf("> %s \t [%s] \n", Items.at(I).Name.c_str(), ((std::vector<std::string>*)(Items.at(I).Data.List.Data))->at(*(Items.at(I).Data.List.Index)).c_str());
					//ImGui::TextColored(ImColor(0, 255, 0), "%s \t [%s]", Items.at(I).Name.c_str(), ((std::vector<std::string>*)(Items.at(I).Data.List.Data))->at(*(Items.at(I).Data.List.Index)).c_str());
					ImGui::TextColored(ImColor(0, 255, 0), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 5);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("[%s]"), ((std::vector<std::string>*)(Items.at(I).Data.List.Data))->at(*(Items.at(I).Data.List.Index)).c_str());
					break;

				case ItemType::Int:
					//printf("> %s \t [%d] \n", Items.at(I).Name.c_str(), *(int*)Items.at(I).Data.Int.Data);
					//ImGui::TextColored(ImColor(0, 255, 0), "%s \t [%d]", Items.at(I).Name.c_str(), *(int*)Items.at(I).Data.Int.Data);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 5);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("[%d]"), *(int*)Items.at(I).Data.Int.Data);
					break;

				case ItemType::Float:
					//printf("> %s \t [%0.3f] \n", Items.at(I).Name.c_str(), *(float*)Items.at(I).Data.Float.Data);
					//ImGui::TextColored(ImColor(0, 255, 0), "%s \t [% 0.3f]", Items.at(I).Name.c_str(), *(float*)Items.at(I).Data.Float.Data);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 5);
					ImGui::TextColored(ImColor(0, 255, 0), xor ("[%0.1f]"), *(float*)Items.at(I).Data.Float.Data);
					break;

				default:
					break;
				}
			}
			else
			{
				switch (Items.at(I).Type)
				{
				case ItemType::Boolean:
					//printf("  %s \t [%s] \n", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off");
					//ImGui::TextColored(ImColor(255, 0, 0), "%s \t [%s]", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off");
					ImGui::TextColored(ImColor(255, 255, 255), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 5);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("[%s]"), *(bool*)(Items.at(I).Data.Boolean.Data) ? xor ("on") : xor ("off"));
					break;
				case ItemType::Arrow:
					//printf("  %s \t [%s] \n", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off");
					//ImGui::TextColored(ImColor(255, 0, 0), "%s \t [%s]", Items.at(I).Name.c_str(), *(bool*)(Items.at(I).Data.Boolean.Data) ? "on" : "off");
					ImGui::TextColored(ImColor(255, 255, 255), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 5);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("[%s]"), *(bool*)(Items.at(I).Data.Boolean.Data) ? xor ("<--") : xor ("..."));
					break;

				case ItemType::List:
					//printf("  %s \t [%s] \n", Items.at(I).Name, ((std::vector<std::string>*)(Items.at(I).Data.List.Data))->at(*(Items.at(I).Data.List.Index)).c_str());
					//ImGui::TextColored(ImColor(255, 0, 0), "%s \t [%s]", Items.at(I).Name.c_str(), ((std::vector<std::string>*)(Items.at(I).Data.List.Data))->at(*(Items.at(I).Data.List.Index)).c_str());
					ImGui::TextColored(ImColor(255, 255, 255), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 5);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("[%s]"), ((std::vector<std::string>*)(Items.at(I).Data.List.Data))->at(*(Items.at(I).Data.List.Index)).c_str());
					break;

				case ItemType::Int:
					//printf("  %s \t [%d] \n", Items.at(I).Name.c_str(), *(int*)Items.at(I).Data.Int.Data);
					//ImGui::TextColored(ImColor(255, 0, 0), "%s \t [%d]", Items.at(I).Name.c_str(), *(int*)Items.at(I).Data.Int.Data);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 5);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("[%d]"), *(int*)Items.at(I).Data.Int.Data);
					break;

				case ItemType::Float:
					//printf("  %s \t [%0.3f] \n", Items.at(I).Name.c_str(), *(float*)Items.at(I).Data.Float.Data);
					//ImGui::TextColored(ImColor(255, 0, 0), "%s \t [% 0.3f]", Items.at(I).Name.c_str(), *(float*)Items.at(I).Data.Float.Data);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("%s"), Items.at(I).Name.c_str());
					ImGui::SameLine();
					ImGui::SetCursorPosX(ImGui::GetWindowPos().x + 5);
					ImGui::TextColored(ImColor(255, 255, 255), xor ("[%0.1f]"), *(float*)Items.at(I).Data.Float.Data);
					break;

				case ItemType::Useless:
					ImGui::TextColored(ImColor(255, 255, 0), xor ("%s"), Items.at(I).Name.c_str());
					break;

				default:
					break;
				}
			}
		}
		ImGui::NewLine();
	}
};
