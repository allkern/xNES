#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

struct key {
	unsigned char kChar = ' ';
	unsigned long kCode = 0ul;
	uint_least8_t kComb = 0  ;
	bool		  kPrsd = false;
	key(unsigned char kChar, unsigned long kCode, bool Alt, bool Ctrl, bool Shift, bool System) {
		this->kChar = kChar;
		this->kCode = kCode;
		this->kComb = (Alt ? (1 << 3) : 0) | (Ctrl ? (1 << 2) : 0) | (Shift ? (1 << 1) : 0) | (System ? 1 : 0);
	}
	key(void) {}

	inline void setComb(bool Alt, bool Ctrl, bool Shift, bool System) {
		this->kComb = (Alt ? (1 << 3) : 0) | (Ctrl ? (1 << 2) : 0) | (Shift ? (1 << 1) : 0) | (System ? 1 : 0);
	}
};

class graphicsWrapper {
private:
	bool __Initialized		  = false, // Initialization guard
		 __EnableScreenBuffer = true;
	sf::RenderWindow* pWindow = nullptr; // Pointer to a window
	[[unused]] sf::Thread* pThread = nullptr; // Pointer to the execution thread
	sf::Font gFont;
	sf::Text gText;
	sf::Color gColor = sf::Color(0, 0, 0);

	std::vector <sf::Font> aFonts;

	unsigned int  __Width, __Height;
	unsigned long __BufferSize;

	sf::Uint8*  __ScreenBuffer;
	sf::Texture __txBuffer;
	sf::Sprite* __spBuffer = new sf::Sprite(__txBuffer);

	key Key;

	inline void __stdcall __grLoop() {
		this->pWindow->setActive(true);
		this->setup();
		while (this->pWindow->isOpen()) {
			sf::Event event;
			while (this->pWindow->pollEvent(event)) {
				switch (event.type) {
					case sf::Event::Closed:
						this->onClose();
						break;
					case sf::Event::TextEntered:
						this->Key.kCode = (unsigned long)event.text.unicode;
						this->onKey();
						break;
					case sf::Event::KeyPressed:
						this->Key.kCode = (unsigned long)event.key.code;
						this->Key.setComb(event.key.alt, event.key.control, event.key.shift, event.key.system);
						this->Key.kPrsd = true;
						this->onSpKey();
						break;
				}
			}
			this->draw();
			if (__EnableScreenBuffer)
				this->__txBuffer.update(this->__ScreenBuffer);
				this->pWindow->draw(*__spBuffer);
			this->pWindow->display();
		}
	}

	void __stdcall constructScreenBuffer() {
		if (__Initialized) {
			this->__BufferSize = (unsigned long)this->__Width * (unsigned long)this->__Height * 4;
			this->__ScreenBuffer = new sf::Uint8[__BufferSize];
			this->__txBuffer.create(this->__Width, this->__Height);
			this->__spBuffer = new sf::Sprite(this->__txBuffer);
			for (int i = 0; i < __BufferSize; i += 4) {
				this->__ScreenBuffer[i]		= (sf::Uint8)0x00; // R
				this->__ScreenBuffer[i + 1] = (sf::Uint8)0x00; // G
				this->__ScreenBuffer[i + 2] = (sf::Uint8)0x00; // B
				this->__ScreenBuffer[i + 3] = (sf::Uint8)0xff; // A
			}
			this->__txBuffer.update(this->__ScreenBuffer);
		}
	}

public:
	__stdcall graphicsWrapper(int __Width, int __Height, std::string __Title, bool __EnableScreenBuffer = false) :
		__Initialized(true),
		__Width(__Width), 
		__Height(__Height),
		__BufferSize((unsigned long)__Width* (unsigned long)__Height * 4) {
		if (__EnableScreenBuffer) constructScreenBuffer();
		this->pWindow = new sf::RenderWindow(sf::VideoMode(__Width, __Height), __Title); 
		this->pWindow->setActive(false); 
	}
	__stdcall graphicsWrapper(sf::RenderWindow* pRenderWindow, bool __EnableScreenBuffer = false) :
		__Initialized(true),
		__Width(__Width),
		__Height(__Height),
		__BufferSize(0) {
		if (__EnableScreenBuffer) constructScreenBuffer();
		__txBuffer.create(__Width, __Height);
		this->pWindow = pRenderWindow; 
		this->pWindow->setActive(false);
	}
	__stdcall graphicsWrapper(void) :
		__Width(0),
		__Height(0),
		__ScreenBuffer(nullptr) {}

	inline void __stdcall init(int __Width, int __Height, std::string __Title, bool __EnableScreenBuffer = false) {
		this->pWindow = new sf::RenderWindow(sf::VideoMode(__Width, __Height), __Title);
		this->pWindow->setActive(false);
		//this->pThread = new sf::Thread()
		this->__Initialized = true;
		this->__Width  = __Width;
		this->__Height = __Height;
		this->__EnableScreenBuffer = __EnableScreenBuffer;
		if (__EnableScreenBuffer) constructScreenBuffer();
		this->__grLoop();
		__Initialized = true;
	}

	inline void __stdcall setTextColor(unsigned char __R, unsigned char __G, unsigned char __B, unsigned char __A = 0xff) {
		this->gColor = sf::Color(__R, __G, __B, __A);
	}

	[[nodiscard]] inline sf::Font* __stdcall initFont(std::string __Fname, int __Size) {
		sf::Font __Temp;
		[[maybe_unused]] int __Ptr = aFonts.size();
		this->aFonts.push_back(__Temp);
		this->aFonts[__Ptr].loadFromFile(__Fname);
		this->gText.setFont(this->aFonts[__Ptr]);
		this->gText.setCharacterSize(__Size);
		return &aFonts[__Ptr];
	}
	
	inline void __stdcall drawString(
		float __PosX,
		float __PosY,
		std::string __String,
		sf::Text::Style __Style = sf::Text::Regular) {
		this->gText.setPosition(sf::Vector2f(__PosX, __PosY));
		this->gText.setString(__String);
		this->gText.setFillColor(this->gColor);
		this->gText.setStyle(__Style);
		this->pWindow->draw(this->gText);
	}

	inline void __stdcall clearScreen(sf::Color __Col = sf::Color(0, 0, 0)) {
		for (int i = 0; i < __BufferSize; i += 4) {
			this->__ScreenBuffer[i]		= __Col.r; // R
			this->__ScreenBuffer[i + 1] = __Col.g; // G
			this->__ScreenBuffer[i + 2] = __Col.b; // B
			this->__ScreenBuffer[i + 3] = __Col.a; // A
		}
	}

	inline void __stdcall drawPoint(
		int __PosX,
		int __PosY,
		sf::Color __Col) {
		int __Base = (__PosX * 4) + (this->__Width * (__PosY * 4));
		this->__ScreenBuffer[__Base  ] = __Col.r;
		this->__ScreenBuffer[__Base+1] = __Col.g;
		this->__ScreenBuffer[__Base+2] = __Col.b;
		this->__ScreenBuffer[__Base+3] = __Col.a;
	}

	inline void __stdcall onKey();
	inline void __stdcall onSpKey();
	inline void __stdcall onClose();
	inline void __stdcall draw();
	inline void __stdcall setup();
};

#define window this->pWindow

typedef graphicsWrapper gw;