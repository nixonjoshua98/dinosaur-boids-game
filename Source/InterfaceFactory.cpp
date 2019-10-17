#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/UI/UI.h>

#include "InterfaceFactory.h"

InterfaceFactory::InterfaceFactory(UI* ui, ResourceCache* _cache)
{
	root = ui->GetRoot();
	cache = _cache;
}

void InterfaceFactory::CreateText(Text*& txt, HorizontalAlignment allign, String name, String font, int size, Vector2 pos, String defaultText)
{
	txt = root->CreateChild<Text>(name);

	txt->SetFont(cache->GetResource<Font>(font), size);
	txt->SetTextAlignment(HA_LEFT);
	txt->SetHorizontalAlignment(allign);
	txt->SetVerticalAlignment(VA_TOP);
	txt->SetPosition(pos.x_, pos.y_);
	txt->SetVisible(false);
	txt->SetText(defaultText);
}
