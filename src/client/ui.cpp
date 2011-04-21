#include "ui.h"
#include "map.h"

namespace {
	scoord toScreenPos(mcoord c) {
		return (scoord)(c >> 8);
	}
	tcoord screenPosToTile(scoord c) {
		return Map::posToTile(mcoord(c) << 8);
	}

	const scoord minScrollLeft = 245;
	const scoord minScrollRight = 245;
	const scoord minScrollTop = 115;
	const scoord minScrollBottom = 215;
}

UI::UI(ResourceManager* res, sf::RenderWindow* win)
	: resources(res), window(win),
	otterAnim(),
	otterUpLeft(Resources::OtterUpLeft, 4, 140),
	otterLeft(Resources::OtterLeft, 4, 140),
	otterDownLeft(Resources::OtterDownLeft, 4, 140),
	hasPlayedDeathSound(false), scrollx(0), scrolly(0)
{
	deathSound.SetBuffer(resources->getSound(Resources::DeathSound));

	const sf::Font& font = resources->getFont(Resources::WalterTurncoat);
	restartText.SetString("Press R to restart.");
	restartText.SetFont(font);
	restartText.SetCharacterSize(50);
	restartText.SetColor(sf::Color(255, 255, 255));
	centerText(restartText, 140);
	restartHeader.SetString("You died.");
	restartHeader.SetFont(font);
	restartHeader.SetCharacterSize(100);
	restartHeader.SetColor(sf::Color(255, 0, 0));
	centerText(restartHeader, 40);
}

void UI::makeEvent(sf::Event e, const GameState& gs,
		std::queue<GameState::Event::Type>& eventQueue) const {
	if (e.Type == sf::Event::KeyReleased) {
		if (e.Key.Code == sf::Key::Up)
			eventQueue.push(GameState::Event::EndLookUp);
		else if (e.Key.Code == sf::Key::Left)
			eventQueue.push(GameState::Event::EndMoveLeft);
		else if (e.Key.Code == sf::Key::Right)
			eventQueue.push(GameState::Event::EndMoveRight);
	}
	else if (e.Type == sf::Event::KeyPressed) {
		if (e.Key.Code == sf::Key::Up)
			eventQueue.push(GameState::Event::BeginLookUp);
		else if (e.Key.Code == sf::Key::Left)
			eventQueue.push(GameState::Event::BeginMoveLeft);
		else if (e.Key.Code == sf::Key::Right)
			eventQueue.push(GameState::Event::BeginMoveRight);
		else if (e.Key.Code == sf::Key::Z)
			eventQueue.push(GameState::Event::Jump);
		else if (e.Key.Code == sf::Key::R)
			eventQueue.push(GameState::Event::Restart);
	}
}

void UI::centerText(sf::Text& str, int y) {
	float w = str.GetRect().Width;
	str.SetY((float)y);
	str.SetX((window->GetWidth() - w) / 2);
}

void UI::drawMapSprite(sf::Sprite& spr, MPosition pos) {
	scoord x = toScreenPos(pos.x) - scrollx;
	scoord y = toScreenPos(pos.y) - scrolly;
	spr.SetPosition((float)x, (float)y);
	window->Draw(spr);
}

void UI::drawCenteredMapSprite(sf::Sprite& spr, MPosition pos) {
	scoord x = toScreenPos(pos.x) - ((int)spr.GetSize().x / 2) - scrollx;
	scoord y = toScreenPos(pos.y) - ((int)spr.GetSize().y / 2) - scrolly;
	spr.SetPosition((float)x, (float)y);
	window->Draw(spr);
}

void UI::adjustScrolling(const Player& pl) {
	MPosition p = pl.getPosition();
	const scoord maxx = window->GetWidth() - minScrollRight;
	const scoord maxy = window->GetHeight() - minScrollBottom;

	scoord oldx = toScreenPos(p.x) - scrollx;
	scoord oldy = toScreenPos(p.y) - scrolly;

	if (oldx < minScrollLeft)
		scrollx -= minScrollLeft - oldx;
	else if (oldx > maxx)
		scrollx -= maxx - oldx;

	if (oldy < minScrollTop)
		scrolly -= minScrollTop - oldy;
	else if (oldy > maxy)
		scrolly -= maxy - oldy;
}

void UI::drawPlayer(const Player& pl) {
	int pose = pl.getPose();
	bool flip = false;

	if (pose < 8) {
		if (pose&1) flip = true;
		pose &= ~1;
		if (pose >= 6) pose -= 6; // temp hack
		if (pose == 0) otterAnim.resetFromDifferent(otterLeft);
		else if (pose == 2) otterAnim.resetFromDifferent(otterUpLeft);
		else if (pose == 4) otterAnim.resetFromDifferent(otterDownLeft);
	}

	sf::Sprite& otterSprite = resources->getImage(otterAnim.getImage());
	if (flip) otterSprite.FlipX(true);
	drawCenteredMapSprite(otterSprite, pl.getPosition());
	if (flip) otterSprite.FlipX(false);
}

void UI::paint(unsigned int delay, const GameState& gs) {
	const Map* map = gs.getMap();
	const Player& pl = gs.getPlayer();

	window->Clear();

	adjustScrolling(pl);

	tcoord fromx = screenPosToTile(scrollx);
	tcoord tox = screenPosToTile(scrollx + window->GetWidth()-1);
	tcoord fromy = screenPosToTile(scrolly);
	tcoord toy = screenPosToTile(scrolly + window->GetHeight()-1);
	for (tcoord tiley = fromy; tiley <= toy; ++tiley) {
		for (tcoord tilex = fromx; tilex <= tox; ++tilex) {
			TPosition p(tilex, tiley);
			const Map::Tile& tile = map->getTileAt(p);
			sf::Sprite& tileSprite = resources->getImage(tile.getImage());
			drawMapSprite(tileSprite, Map::tileToPos(p));
		}
	}

	otterAnim.step(delay);

	gs.drawObjects(*this);

	if (gs.deadPlayer()) {
		if (!hasPlayedDeathSound) {
			deathSound.Play();
			hasPlayedDeathSound = true;
		}
		window->Draw(restartHeader);
		window->Draw(restartText);
	}
	else {
		// Reset the death state
		hasPlayedDeathSound = false;
	}
}
