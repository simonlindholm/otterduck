#include "resources.h"
#include "exceptions.h"
using namespace Resources;

void ResourceManager::set32Handle(int x, int y, sf::Sprite* spr,
		Img id) {
	ImgHandle ret;
	ret.spr = spr;
	ret.subrect.Left = x;
	ret.subrect.Top = y;
	ret.subrect.Width = ret.subrect.Height = 32;
	imgRes[id] = ret;
}
void ResourceManager::setAnim32Handle(int x, int y, sf::Sprite* spr,
		Img from, unsigned int num) {
	for (unsigned int i = 0; i < num; ++i) {
		set32Handle(x + i*32, y, spr, from);
		from = (Img)((int)from + 1);
	}
}

ResourceManager::ResourceManager() {
	if (!tilesImg.LoadFromFile("img/tiles.png") ||
	    !otterImg.LoadFromFile("img/otter-evil.png") ||
	    !itemsImg.LoadFromFile("img/items.png") ||
	    !soRes[DeathSound].LoadFromFile("sound/death.wav") ||
		!fontRes[WalterTurncoat].LoadFromFile("WalterTurncoat.ttf")) {
		throw ConfigException(
				"Failed to load resources. (Are you running from bin?)");
	}

	tilesImg.SetSmooth(false);
	otterImg.SetSmooth(false);
	itemsImg.SetSmooth(false);
	tiles.SetImage(tilesImg);
	otter.SetImage(otterImg);
	items.SetImage(itemsImg);

	setAnim32Handle(0, 32, &otter, OtterUp, 4);
	setAnim32Handle(0, 32, &otter, OtterUpLeft, 4);
	setAnim32Handle(0, 64, &otter, OtterLeft, 4);
	setAnim32Handle(0, 96, &otter, OtterDownLeft, 4);
	setAnim32Handle(0, 128, &otter, OtterDown, 4);

	set32Handle(0, 0, &tiles, TileBlock);
	set32Handle(32, 0, &tiles, TileEmpty);
	set32Handle(64, 0, &tiles, TileSpikes);

	set32Handle(0, 0, &items, ItemGravity);
}

sf::Sprite& ResourceManager::getImage(Img id) const {
	ImgHandle h = imgRes[id];
	h.spr->SetSubRect(h.subrect);
	return *h.spr;
}

const sf::SoundBuffer& ResourceManager::getSound(Sound id) const {
	return soRes[id];
}

const sf::Font& ResourceManager::getFont(Resources::Font id) const {
	return fontRes[id];
}
