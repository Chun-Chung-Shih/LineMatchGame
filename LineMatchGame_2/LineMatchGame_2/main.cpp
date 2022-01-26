#include <SFML/Graphics.hpp>

int main() {
	const int fieldWidth = 10;
	const int fieldHeight = 20;

	enum class Type {
		None,
		O,
		I,
		L,
	};

	Type field[fieldWidth][fieldHeight] = {};

	sf::Texture backgroundTexture;
	if (!backgroundTexture.loadFromFile("background.png")) {
		return EXIT_FAILURE;
	}

	sf::Texture yellowTexture;
	if (!yellowTexture.loadFromFile("yellow.png")) {
		return EXIT_FAILURE;
	}

	sf::Texture lightBlueTexture;
	if (!lightBlueTexture.loadFromFile("light_blue.png")) {
		return EXIT_FAILURE;
	}

	sf::Texture redTexture;
	if (!redTexture.loadFromFile("red.png")) {
		return EXIT_FAILURE;
	}

	std::map<Type, std::vector<std::vector<sf::Vector2i>>> shapes = {
		{
			Type::O,
			{
		// O:     OO    (0,-1) (1,-1)
		//        OO    (0, 0) (1, 0)
		{
			sf::Vector2i(0, 0),
			sf::Vector2i(1, 0),
			sf::Vector2i(0,-1),
			sf::Vector2i(1,-1),
		}
	},
},
{
	Type::I,
	{
		// I:     OOOO   (-1, 0) (0, 0) (1,0) (2,0)  shape_Is[0]
		{
			sf::Vector2i(-1, 0),
			sf::Vector2i(0, 0),
			sf::Vector2i(1, 0),
			sf::Vector2i(2, 0),
		},
		// I:     O   (0,-2)                         shape_Is[1]
		//        O   (0,-1)
		//        O   (0, 0)
		//        O   (0, 1)
		{
			sf::Vector2i(0,-2),
			sf::Vector2i(0,-1),
			sf::Vector2i(0, 0),
			sf::Vector2i(0, 1),
		}
	}

	},
	{
		Type::L,
		{
		//L: 
		//	  O	(0,-2)
		//	  O	(0, -1)
		//	  O	O   (0, 0)(1, 0)
			{
			sf::Vector2i(0,-2),
			sf::Vector2i(0,-1),
			sf::Vector2i(0, 0),
			sf::Vector2i(1, 0),
			},
			{
				//	    O
				//	    O
				//O	O

			sf::Vector2i(0,-2),
			sf::Vector2i(0,-1),
			sf::Vector2i(0, 0),
			sf::Vector2i(-1, 0),
			},
			{
				//O	O
				//		O
				//		O
			sf::Vector2i(-1,-2),
			sf::Vector2i(0,-2),
			sf::Vector2i(0, -1),
			sf::Vector2i(0, 0),
			},
			{
				//O	O
				//O
				//O
			sf::Vector2i(1,-2),
			sf::Vector2i(0,-2),
			sf::Vector2i(0, -1),
			sf::Vector2i(0, 0),
			},
		}

	}
	};

	Type currentType = Type(rand() % 3 + 1);
	int currentIndex = 0;

	const int blockWidth = yellowTexture.getSize().x;
	const int blockHeight = yellowTexture.getSize().y;

	const int windowWidth = fieldWidth * blockWidth;
	const int windowHeight = fieldHeight * blockHeight;

	std::map<Type, sf::Sprite> sprites = {
		{Type::O, sf::Sprite(yellowTexture)},
		{Type::I, sf::Sprite(lightBlueTexture)},
		{Type::L, sf::Sprite(redTexture)},
	};

	backgroundTexture.setRepeated(true);
	sf::Sprite backgroundSprite(
		backgroundTexture,
		sf::IntRect(0, 0, windowWidth, windowHeight));

	std::vector<sf::Vector2i> currentShape;
	sf::Sprite currentSprite;

	const sf::Vector2i origin(fieldWidth / 2, 0);
	sf::Vector2i pos(origin);

	sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), L"俄羅斯方塊");
	sf::Clock clock;
	while (window.isOpen()) {
		currentShape = shapes[currentType][currentIndex];
		currentSprite = sprites[currentType];

		enum class Action {
			Hold,
			MoveDown,
			MoveLeft,
			MoveRight,
			Rotate,
		};
		Action action = Action::Hold;
		sf::Event evt;
		if (window.pollEvent(evt)) {
			if (evt.type == sf::Event::Closed) {
				window.close();
			}
			if (evt.type == sf::Event::KeyPressed) {
				switch (evt.key.code) {
				case sf::Keyboard::Left:
					action = Action::MoveLeft;
					break;
				case sf::Keyboard::Right:
					action = Action::MoveRight;
					break;
				case sf::Keyboard::Up:
					action = Action::Rotate;
					break;
				case sf::Keyboard::Down:
					action = Action::MoveDown;
					break;
				}
			}
		}
		if (clock.getElapsedTime().asSeconds() >= 0.3f) {
			action = Action::MoveDown;
			clock.restart();
		}

		sf::Vector2i nextPos(pos);
		int nextIndex = currentIndex;
		if (action == Action::Rotate) {
			// 0 => 1 => 0 => 1 ...
			nextIndex = (nextIndex + 1) % shapes[currentType].size();
		}
		std::vector<sf::Vector2i> nextShape = shapes[currentType][nextIndex];
		switch (action) {
		case Action::Hold:
			break;
		case Action::MoveDown:
			nextPos.y++;
			break;
		case Action::MoveLeft:
			nextPos.x--;
			break;
		case Action::MoveRight:
			nextPos.x++;
			break;
		}

		int countEmpty = 0;
		for (const sf::Vector2i d : nextShape) {
			sf::Vector2i np = nextPos + d;
			if (np.x >= 0 && np.x < fieldWidth &&
				np.y < fieldHeight &&
				(np.y < 0 || field[np.x][np.y] == Type::None)) {
				countEmpty++;
			}
		}

		if (countEmpty == 4) {
			pos = nextPos;
			currentIndex = nextIndex;
			currentShape = nextShape;
		}
		else {
			if (action == Action::MoveDown) {
				for (const sf::Vector2i& d : currentShape) {
					sf::Vector2i np = pos + d;
					if (np.y >= 0) {
						field[np.x][np.y] = currentType;
					}
				}

				for (int y = 0; y < fieldHeight; y++) {
					bool isFull = true;
					for (int x = 0; x < fieldWidth; x++) {
						if (field[x][y] == Type::None) {
							isFull = false;
						}
					}
					if (isFull) {
						for (int ty = y; ty > 0; ty--) {
							for (int x = 0; x < fieldWidth; x++) {
								field[x][ty] = field[x][ty - 1];
							}
						}
						for (int x = 0; x < fieldWidth; x++) {
							field[x][0] = Type::None;
						}
					}
				}

				pos = origin;
				currentType = Type(rand() % 3 + 1);
				currentIndex = 0;
			}
		}
		window.clear();

		window.draw(backgroundSprite);

		// 繪製背景的場地
		for (int x = 0; x < fieldWidth; x++) {
			for (int y = 0; y < fieldHeight; y++) {
				if (field[x][y] == Type::None) continue;

				sf::Sprite& s = sprites[field[x][y]];

				s.setPosition(
					float(x* blockWidth),
					float(y* blockHeight));
				window.draw(s);
			}
		}

		// 繪製控制的物體
		// O:     OO    (0,-1) (1,-1)
		//        OO    (0, 0) (1, 0)
		for (const sf::Vector2i& d : currentShape) {
			sf::Vector2i np = pos + d;
			currentSprite.setPosition(
				float(np.x* blockWidth),
				float(np.y* blockHeight));
			window.draw(currentSprite);
		}
		window.display();
	}
	return EXIT_SUCCESS;
}