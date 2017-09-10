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
	winDimensions(win->getView().getSize()),
	otterAnim(),
	otterUpLeft(Resources::OtterUpLeft, 4, 140),
	otterLeft(Resources::OtterLeft, 4, 140),
	otterDownLeft(Resources::OtterDownLeft, 4, 140),
	hasPlayedDeathSound(false), scrollx(0), scrolly(0)
{
	deathSound.setBuffer(resources->getSound(Resources::DeathSound));

	const sf::Font& font = resources->getFont(Resources::WalterTurncoat);
	restartText.setString("Press R to restart.");
	restartText.setFont(font);
	restartText.setCharacterSize(50);
	restartText.setColor(sf::Color(255, 255, 255));
	centerText(restartText, 140);
	restartHeader.setString("You died.");
	restartHeader.setFont(font);
	restartHeader.setCharacterSize(100);
	restartHeader.setColor(sf::Color(255, 0, 0));
	centerText(restartHeader, 40);
}

void UI::makeEvent(sf::Event e, const GameState& gs,
		std::queue<GameState::Event::Type>& eventQueue) const {
	if (e.type == sf::Event::KeyReleased) {
		if (e.key.code == sf::Keyboard::Up)
			eventQueue.push(GameState::Event::EndLookUp);
		else if (e.key.code == sf::Keyboard::Left)
			eventQueue.push(GameState::Event::EndMoveLeft);
		else if (e.key.code == sf::Keyboard::Right)
			eventQueue.push(GameState::Event::EndMoveRight);
	}
	else if (e.type == sf::Event::KeyPressed) {
		if (e.key.code == sf::Keyboard::Up)
			eventQueue.push(GameState::Event::BeginLookUp);
		else if (e.key.code == sf::Keyboard::Left)
			eventQueue.push(GameState::Event::BeginMoveLeft);
		else if (e.key.code == sf::Keyboard::Right)
			eventQueue.push(GameState::Event::BeginMoveRight);
		else if (e.key.code == sf::Keyboard::Z || e.key.code == sf::Keyboard::Space)
			eventQueue.push(GameState::Event::Jump);
		else if (e.key.code == sf::Keyboard::R)
			eventQueue.push(GameState::Event::Restart);
	}
}

void UI::centerText(sf::Text& str, int y) {
	float w = str.getLocalBounds().width;
	str.setPosition((winDimensions.x - w) / 2, (float)y);
	// float w = str.getPos().width;
	// str.setY((float)y);
	// str.setX((window->getWidth() - w) / 2);
}

void UI::drawMapSprite(sf::Sprite& spr, MPosition pos) {
	scoord x = toScreenPos(pos.x) - scrollx;
	scoord y = toScreenPos(pos.y) - scrolly;
	spr.setPosition((float)x, (float)y);
	window->draw(spr);
}

void UI::drawCenteredMapSprite(sf::Sprite& spr, MPosition pos, bool flipX) {
	// getSize -> getLocalBounds
	int w = (int)spr.getLocalBounds().width;
	int h = (int)spr.getLocalBounds().width;
	scoord x = toScreenPos(pos.x) - (w / 2) - scrollx;
	scoord y = toScreenPos(pos.y) - (h / 2) - scrolly;
	spr.setPosition((float)x + (flipX ? w : 0), (float)y);
	if (flipX) spr.setScale(-1, 1);
	window->draw(spr);
	if (flipX) spr.setScale(1, 1);
}

void UI::adjustScrolling(const Player& pl) {
	MPosition p = pl.getPosition();
	const scoord maxx = winDimensions.x - minScrollRight;
	const scoord maxy = winDimensions.y - minScrollBottom;

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
	drawCenteredMapSprite(otterSprite, pl.getPosition(), flip);
}

void UI::paint(unsigned int delay, const GameState& gs) {
	const Map* map = gs.getMap();
	const Player& pl = gs.getPlayer();

	window->clear();

	adjustScrolling(pl);

	tcoord fromx = screenPosToTile(scrollx);
	tcoord tox = screenPosToTile(scrollx + winDimensions.x-1);
	tcoord fromy = screenPosToTile(scrolly);
	tcoord toy = screenPosToTile(scrolly + winDimensions.y-1);
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
			deathSound.play();
			hasPlayedDeathSound = true;
		}
		window->draw(restartHeader);
		window->draw(restartText);
	}
	else {
		// Reset the death state
		hasPlayedDeathSound = false;
	}
}
