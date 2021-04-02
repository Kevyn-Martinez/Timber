#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

//Function declaration
void updateBranches(int seed);
const int NUM_BRANCHES = 6;
Sprite branches[NUM_BRANCHES];

//Where is the player/branch?
//Left right?

enum class side{LEFT, RIGHT, NONE};
side branchPositions[NUM_BRANCHES];

int main()
{
	//create a video mode object (the screen)
	//VideoMode vm(1920, 1080);
	
	//Low Res Mode
	VideoMode vm(1280, 720);
	//Create and open a window for the game
	//RenderWindow window(vm, "Timber!!!", Style::Fullscreen);

	//Low res code
	RenderWindow window(vm, "Timber!!");
	View view(FloatRect(0, 0, 1920, 1080));
	window.setView(view);

	//Create a texture to hold a graphic on the GPU
	Texture textureBackground;

	//Load a graphic into the texture
	textureBackground.loadFromFile("graphics/background.png");

	//Create a sprite
	Sprite spriteBackground;

	//Attach the texture to the sprite
	spriteBackground.setTexture(textureBackground);

	//Set the spriteBackground to cover the screen
	spriteBackground.setPosition(0, 0);

	//Make a tree sprite
	Texture textureTree;
	textureTree.loadFromFile("graphics/tree.png");
	Sprite spriteTree;
	spriteTree.setTexture(textureTree);
	spriteTree.setPosition(810, 0);

	//Prepare the bee
	Texture textureBee;
	textureBee.loadFromFile("graphics/bee.png");
	Sprite spriteBee;
	spriteBee.setTexture(textureBee);
	spriteBee.setPosition(0, 800);

	//Is this currently moving?
	bool beeActive = false;
	//How fast can the bee fly?
	float beeSpeed = 0.0f;

	//Make 3 cloud sprites from 1 texture
	Texture textureCloud;
	textureCloud.loadFromFile("graphics/cloud.png");

	//3 New Sprites with the same texture
	Sprite spriteCloud1, spriteCloud2, spriteCloud3;
	spriteCloud1.setTexture(textureCloud);
	spriteCloud2.setTexture(textureCloud);
	spriteCloud3.setTexture(textureCloud);

	//Position of the clouds off screen
	spriteCloud1.setPosition(0, 0);
	spriteCloud2.setPosition(0, 250);
	spriteCloud3.setPosition(0, 500);
	
	//Are the clouds currently on screen?
	bool cloud1Active = false, cloud2Active = false, cloud3Active = false;

	//How fast is each cloud?
	float cloud1Speed = 0.0f, cloud2Speed = 0.0f, cloud3Speed = 0.0f;

	//Variables to control time itself
	Clock clock;

	//Time Bar
	RectangleShape timeBar;
	float timeBarStartWidth = 400, timeBarHeight = 80;
	timeBar.setSize(Vector2f(timeBarStartWidth, timeBarHeight));
	timeBar.setFillColor(Color::Red);
	timeBar.setPosition((1920 / 2) - timeBarStartWidth / 2, 980);

	Time gameTimeTotal;
	float timeRemaining = 6.0f, timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

	//Track whether the game is running
	bool paused = true;

	//Draw some Text
	int score = 0;
	Text messageText, scoreText;

	//We need to choose a font
	Font font;
	font.loadFromFile("fonts/KOMIKAP_.ttf");
	
	//Set the font to our message
	messageText.setFont(font);
	scoreText.setFont(font);

	//Assign the actual message
	messageText.setString("Press Enter to start!");
	scoreText.setString("Score =0");

	//Make text bigger
	messageText.setCharacterSize(75);
	scoreText.setCharacterSize(100);

	//Choose a color
	messageText.setFillColor(Color::White);
	scoreText.setFillColor(Color:: White);

	//Position the text
	FloatRect textRect = messageText.getLocalBounds();
	messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
	scoreText.setPosition(20, 20);

	//Prepare 6 branches
	Texture textureBranch;
	textureBranch.loadFromFile("graphics/branch.png");

	//Set the texture for each branch sprite
	for (int i = 0; i < NUM_BRANCHES; i++) {
		branches[i].setTexture(textureBranch);
		branches[i].setPosition(-2000, -2000);

		//set the sprite's origin to dead center we can then spin it around without changing its position
		branches[i].setOrigin(220, 20);
	}

	//Prepare the player
	Texture texturePlayer;
	texturePlayer.loadFromFile("graphics/player.png");
	Sprite spritePlayer;
	spritePlayer.setTexture(texturePlayer);
	spritePlayer.setPosition(580, 720);

	//The player starts on the left
	side playerSide = side::LEFT;

	//Pepare the gravestone
	Texture textureRIP;
	textureRIP.loadFromFile("graphics/rip.png");
	Sprite spriteRIP;
	spriteRIP.setTexture(textureRIP);
	spriteRIP.setPosition(600, 860);

	//Prepare the axe
	Texture textureAxe;
	textureAxe.loadFromFile("graphics/axe.png");
	Sprite spriteAxe;
	spriteAxe.setTexture(textureAxe);
	spriteAxe.setPosition(700, 830);

	//Line the axe up with the tree
	const float AXE_POSITION_LEFT = 700, AXE_POSITION_RIGHT = 1075;

	//Prepare flying log
	Texture textureLog;
	textureLog.loadFromFile("graphics/log.png");
	Sprite spriteLog;
	spriteLog.setTexture(textureLog);
	spriteLog.setPosition(810, 720);

	//Some other useful log related variables
	bool logActive = false;
	float logSpeedX = 100, logSpeedY = -1500;

	//Control the player input
	bool acceptInput = false;

	//Prepare the sound
	SoundBuffer chopBuffer;
	chopBuffer.loadFromFile("sound/chop.wav");
	Sound chop;
	chop.setBuffer(chopBuffer);

	SoundBuffer deathBuffer;
	deathBuffer.loadFromFile("sound/death.wav");
	Sound death;
	death.setBuffer(deathBuffer);

	//Out of time
	SoundBuffer ootBuffer;
	ootBuffer.loadFromFile("sound/out_of_time.wave");
	Sound outOfTime;
	outOfTime.setBuffer(ootBuffer);

	while (window.isOpen()) {
		//Handle the player inpute

		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::KeyReleased && !paused) {
				//Listen for key pressed again
				acceptInput = true;

				//Hide the axe
				spriteAxe.setPosition(2000, spriteAxe.getPosition().y);
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::Escape))
			window.close();

		//Start the game
		if (Keyboard::isKeyPressed(Keyboard::Return)) {
			paused = false;

			//Reset the time and the score
			score = 0;
			timeRemaining = 5;

			//Mske all the branches dissapear
			for (int i = 0; i < NUM_BRANCHES; i++) {
				branchPositions[i] = side::NONE;
			}

			//Make sure gravestone is hidden
			spriteRIP.setPosition(675, 2000);

			//Move the player into position
			spritePlayer.setPosition(580, 720);
			acceptInput = true;
		}

		//Wrap the player controls to make sure we are accepting input
		if (acceptInput) {
			//First handle pressing right cursor key
			if (Keyboard::isKeyPressed(Keyboard::Right)) {
				//Make sure the player is on the right
				playerSide = side::RIGHT;
				score++;

				//Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;

				spriteAxe.setPosition(AXE_POSITION_RIGHT, spriteAxe.getPosition().y);
				spritePlayer.setPosition(1200, 700);

				//update the branches
				updateBranches(score);
				
				//set the log flying to the left
				spriteLog.setPosition(810, 720);
				logSpeedX = -5000;
				logActive = true;

				acceptInput = false;
				chop.play();
			}

			//Handle the left cursor key
			if (Keyboard::isKeyPressed(Keyboard::Left)) {
				//Make sure the player is on the left
				playerSide = side::LEFT;
				score++;

				//Add to the amount of time remaining
				timeRemaining += (2 / score) + .15;

				spriteAxe.setPosition(AXE_POSITION_LEFT, spriteAxe.getPosition().y);

				spritePlayer.setPosition(580, 720);

				//update  the branches
				updateBranches(score);

				//set the flying log
				spriteLog.setPosition(810, 720);
				logSpeedX = 5000;
				logActive = true;

				acceptInput = false;
				chop.play();
			}
		}
		//Update the scene
		if (!paused) {
		//Measure time
		Time dt = clock.restart();

		//Subtract from the amount of time Remaining
		timeRemaining -= dt.asSeconds();

		//Size up the time bar
		timeBar.setSize(Vector2f(timeBarWidthPerSecond * timeRemaining, timeBarHeight));

		if (timeRemaining <= 0.0f) {
			//Pause the game
			paused = true;

			//Change the message shown to the player
			messageText.setString("Out of time!!");

			//Reposition the text based on its new size
			FloatRect textRect = messageText.getLocalBounds();
			messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
			messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);

			outOfTime.play();
		}

		//Setup the bee
		if (!beeActive) {

			//How fast is the bee
			srand((int)time(0) * 10);
			beeSpeed = (rand() % 200) + 200;

			//How high is the bee
			srand((int)time(0) * 10);
			float height = (rand() % 500) + 500;
			spriteBee.setPosition(2000, height);
			beeActive = true;
		}
		else {
			//move the bee
			spriteBee.setPosition(spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()), spriteBee.getPosition().y);

			//Has the bee reached the right hand edge of the screen?
			if (spriteBee.getPosition().x < -100) {

				//Set it up ready to be a whole new bee/cloud next frame
				beeActive = false;
			}
		}

		//Manage the clouds
		//cloud 1
		if (!cloud1Active) {

			//How fast is the cloud
			srand((int)time(0) * 10);
			cloud1Speed = (rand() % 200);

			//How high is the cloud

			srand((int)time(0) * 10);
			float height = (rand() % 150);
			spriteCloud1.setPosition(-200, height);
			cloud1Active = true;
		}
		else {
			spriteCloud1.setPosition(spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()), spriteCloud1.getPosition().y);

			//Has the cloud reached the right hand edge of the screen?
			if (spriteCloud1.getPosition().x > 1920) {

				//Set it up ready to be a whole new cloud next frame
				cloud1Active = false;
			}
		}

		//cloud 2
		if (!cloud2Active) {

			//How fast is the cloud
			srand((int)time(0) * 20);
			cloud2Speed = (rand() % 200);

			//How high is the cloud
			srand((int)time(0) * 20);
			float height = (rand() % 300) - 150;
			spriteCloud2.setPosition(-200, height);
			cloud2Active = true;
		}
		else {
			spriteCloud2.setPosition(spriteCloud2.getPosition().x + (cloud2Speed * dt.asSeconds()), spriteCloud2.getPosition().y);

			//Has the cloud reached the right hand edge of the screen?
			if (spriteCloud2.getPosition().x > 1920) {

				//Set it up ready to be a whole new cloud next frame
				cloud2Active = false;
			}
		}

		//cloud 3
		if (!cloud3Active) {

			//How fast is the cloud
			srand((int)time(0) * 30);
			cloud3Speed = (rand() % 200);

			//How high is the cloud
			srand((int)time(0) * 30);
			float height = (rand() % 450) - 150;
			spriteCloud3.setPosition(-200, height);
			cloud3Active = true;
		}
		else {
			spriteCloud3.setPosition(spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()), spriteCloud3.getPosition().y);

			//Has the cloud reached the right hand edge of the screen?
			if (spriteCloud3.getPosition().x > 1920) {

				//Set it up ready to be a whole new cloud next frame
				cloud3Active = false;
			}
		}

		//Update the score text
		stringstream ss;
		ss << " Score = " << score;
		scoreText.setString(ss.str());

		//Update the branch sprites
		for (int i = 0; i < NUM_BRANCHES; i++) {
			float height = i * 150;
			if (branchPositions[i] == side::LEFT) {
				//Move the sprite to the left side
				branches[i].setPosition(610, height);
				
				//flip the sprite around the other way
				branches[i].setRotation(180);
			}
			else if (branchPositions[i] == side::RIGHT) {
				//move the sprite to the right side
				branches[i].setPosition(1330, height);

				//set the sprite rotation to normal
				branches[i].setRotation(0);
			}
			else {
				//Hide the branch
				branches[i].setPosition(3000, height);
			}
		}

		//Handle the flying log
		if (logActive) {
			spriteLog.setPosition(spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()), spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()));

			//has the log reached the right hand edge?
			if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000) {
				//Set it up to be a whole new log next frame
				logActive = false;
				spriteLog.setPosition(810, 720);
			}
		}

		//Has the player been squished by a branch?
		if (branchPositions[5] == playerSide) {
			//death
			paused = true;
			acceptInput = false;

			//Draw the graveStone
			spriteRIP.setPosition(spritePlayer.getPosition());

			//hide The Player
			spritePlayer.setPosition(2000, 660);

			//hide the flying log
			spriteLog.setPosition(2000, 600);
			//Change the text of the message
			messageText.setString("SQUISHED!!");

			//Center it on the screen
			FloatRect textRect = messageText.getLocalBounds();

			messageText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

			messageText.setPosition(1920 / 2.0f, 1080 / 2.0f);
			death.play();
		}
	} // end of Paused If...
		//Draw the scene

		//Clear everything from the last frame
		window.clear();

		//Draw our game scene here
		window.draw(spriteBackground);

		//Draw the Clouds
		window.draw(spriteCloud1);
		window.draw(spriteCloud2);
		window.draw(spriteCloud3);

		//Draw the branches
		for (int i = 0; i < NUM_BRANCHES; i++) {
			window.draw(branches[i]);
		}

		//Draw the tree
		window.draw(spriteTree);

		//Draw the player
		window.draw(spritePlayer);

		//Draw the axe
		window.draw(spriteAxe);

		//Draw the flying log
		window.draw(spriteLog);

		//draw the gravestone
		window.draw(spriteRIP);


		//Draw the Bee (Insect)
		window.draw(spriteBee);

		//Draw the score
		window.draw(scoreText);

		//Draw the timebar
		window.draw(timeBar);

		if (paused) {
			//Draw our message
			window.draw(messageText);
		}

		//Show everything we just drew
		window.display();
	}

	return 0;
}

//Funtion definition
void updateBranches(int seed) {
	//move all teh branches down one place
	for (int j = NUM_BRANCHES - 1; j > 0; j--) {
		branchPositions[j] = branchPositions[j - 1];
	}
	
	//Spawn a new branch at position 0 LEFT RIGHT or NONE
	srand((int)time(0) + seed);
	int r = (rand() % 5);
	switch(r) {
		case 0:
			branchPositions[0] = side::LEFT;
			break;
		case 1:
			branchPositions[1] = side::RIGHT;
			break;
		default:
			branchPositions[0] = side::NONE;
			break;
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
