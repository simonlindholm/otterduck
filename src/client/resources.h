#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>

namespace Resources {
	enum Img {
		OtterUp = 0,
		OtterUpLeft = 4,
		OtterLeft = 8,
		OtterDownLeft = 12,
		OtterDown = 16,
		TileEmpty = 20,
		TileBlock,
		TileSpikes,
		ImgEnd
	};
	enum Sound {
		DeathSound,
		SoundEnd
	};
	enum Font {
		WalterTurncoat,
		FontEnd
	};
};

class ResourceManager {
	public:
		ResourceManager();
		sf::Sprite& getImage(Resources::Img id) const;
		const sf::SoundBuffer& getSound(Resources::Sound id) const;
		const sf::Font& getFont(Resources::Font id) const;

	private:
		struct ImgHandle {
			sf::Sprite* spr;
			sf::IntRect subrect;
		};
		void setAnim32Handle(int x, int y, sf::Sprite* spr,
				Resources::Img from, unsigned int num);
		void set32Handle(int x, int y, sf::Sprite* spr, Resources::Img id);
		ImgHandle imgRes[Resources::ImgEnd];
		sf::Image tilesImg, otterImg;
		sf::Sprite tiles, otter;

		sf::SoundBuffer soRes[Resources::SoundEnd];
		sf::Font fontRes[Resources::FontEnd];
};
