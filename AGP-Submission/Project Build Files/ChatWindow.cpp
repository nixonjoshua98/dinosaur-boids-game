#include "ChatWindow.h"

#include <iostream>

ChatWindow::ChatWindow(UI* _ui, ResourceCache* _cache) : InterfaceBaseClass(_ui, _cache)
{
	Create();
}

void ChatWindow::Create()
{
	if (isCreated) return;

	window		= CreateWindow("ChatWindow", LM_VERTICAL, { 300, 250 }, 312);
	chatView	= CreateListView(window, 96);

	msgBox	= CreateLineEdit(window, "");
	sendBtn = CreateButton(window, "Send");

	sendBtn->SetFixedHeight(24);
	window->SetFixedWidth(312);
	chatView->SetFixedWidth(312);

	root->AddChild(window);

	isCreated = true;
}

void ChatWindow::AddMessage(String str)
{
	Text* txt = CreateText(window, str);

	chatView->AddItem(txt);

	if (!chatView->HasFocus())
	{
		chatView->SetSelection(chatView->GetNumItems() - 1);
	}
}

void ChatWindow::Clear()
{
	chatView->RemoveAllItems();
}
