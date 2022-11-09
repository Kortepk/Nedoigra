#include <SFML/Graphics.hpp>  
#include <SFML/Audio.hpp>
#include "SFML/Audio/Music.hpp"
#include "SFML/Audio/Listener.hpp"
#include <iostream>
using namespace sf;  
using namespace std;  
int window_width = 1200, window_height = 800; // разрешения экрана

int mass = 50; //масса
int user_speed = 3; // скорость налево/направо
int wall_health = 100; // здоровье стены
int user_health = 100; // здоровье игрока (которое постояно)
int scene = 0; // 0 - сцена меню; 1 - сцена игры

float user_x = 600, user_y = 100; // коорды. игрока
float delta_x = 0, delta_y = 0; // во время нажатия отслеживается разница между центром игроком и курсором
float g = 9.8; // ускорение)
float V_x = 0, V_y = 0; // вектора скорости
float user_time = 0.1; // время между кадрами
float power_jump = 75; // сила прыжка

Vector2u texture_size; // размеры текстуры игрока

string face_user = "none"; // показывает куда направляется игрок

bool click = false; // если произошло нажатие - true, иначе false
bool interaction_user = false;
bool blocked_direction[4] = {false, false, false, false}; // 0 - вверх 1 - вправо 2 - вниз 3 - вверх
bool brick_live = true;
bool update_menu = true;

RenderWindow window(VideoMode(window_width, window_height), "Game", Style::Default);  // настройка окна игры

void check_board(){ // проверка на границы экрана и игрока
	if(texture_size.x/2 > user_x)
		user_x = texture_size.x/2; // сдвиг на пол текстуры по длине
	if(user_x > window_width - texture_size.x/2)
		user_x = window_width - texture_size.x/2; // сдвиг на пол текстуры по длине
	if(user_y >= window_height)
		user_y = window_height;	// сдвиг до нижней части экрана
}

void Update(){ // процедура на "физику"
	if(interaction_user) { // это, если мы взаимодействуем с игроком (нажали на него)
		V_x = 0;
		V_y = 0;
		return;
	}
	if(mass == 0) return; // если нулевая масс - то нету падения/физики
		
	if((not blocked_direction[2] or (V_y < 0 and not blocked_direction[0])) and (user_y < window_height or V_y < 0)){ // проверку на массу и выход за границы и если скорость отрицательна, то игрок прыгает
		V_y += g * user_time; // физика
		user_y += V_y * user_time; //физика и передвигаем игрока
		
		if(user_y >= window_height){ // если вышли за экран
			user_y = window_height;
			V_y = 0;
		}
	}
	if(blocked_direction[2] and V_y > 0 )
		V_y = 0;
		
	if(V_x != 0 and (texture_size.x/2< user_x or V_x > 0) and (user_x < window_width - texture_size.x/2 or V_x < 0) ){ // проверка на скорость, границы
		if((blocked_direction[3] and V_x > 0) or (blocked_direction[1] and V_x < 0) or (not blocked_direction[3] and not blocked_direction[1]))
			user_x += V_x; // передвигаем игрока
		check_board(); // проверка на границы
	}
}

void render_sun(){
	Texture sun_texture; // создание текстуры игрока
	sun_texture.loadFromFile("source/sun.png"); // загрузить в текстуру картинку 
 	 	
	Sprite sun_sprite(sun_texture); // загружаем в спрайт - текстуру (картинку)
	sun_sprite.setPosition(window_width/2 - 150, 0); //устанавливаем на позицию игрока
	
	window.draw(sun_sprite);
}

void Render(){ // отрисовываем все объекты
	
	render_sun();

	blocked_direction[0] = false; // изначально разблокироваем движение
	blocked_direction[1] = false;
	blocked_direction[2] = false;
	blocked_direction[3] = false;
	
	Texture player_texture; // создание текстуры игрока
	player_texture.loadFromFile("source/mario.png"); // загрузить в текстуру картинку 
 	
 	texture_size = player_texture.getSize(); // получение размеры изображения texture_size.x texture_size.н
 	
	Sprite user_sprite(player_texture); // загружаем в спрайт - текстуру (картинку)
	user_sprite.setPosition(user_x - texture_size.x/2, user_y - texture_size.y); //устанавливаем на позицию игрока
	
	if(face_user == "right"){ // если игрок повернулся направо, то отражаем картинку
		user_sprite.setScale(-1, 1); // отражение спрайта
		user_sprite.move(texture_size.x, 0); // сдвигаем спрайт (так нужно)
	}
	//отрисовка игрока идёт в конце			
      
	                                       
	Texture wall_texture; // создание текстуры
	wall_texture.loadFromFile("source/brick.png"); // загрузить в текстуру картинку 
 	
	Sprite wall_sprite(wall_texture); // загружаем в спрайт - текстуру (картинку) 
	wall_sprite.setPosition(0, 600); //устанавливаем на позицию 
	for(int i = 0; i < window_width/100; i++){ // создаём платформу
		if(i == window_width/100 - 2) // если последний кубик, то переноси наверх (делаю через свою шизу)
			wall_sprite.setPosition(200, 500);
		if(i == window_width/100 - 1 and brick_live)
			wall_sprite.setPosition(200, 400);
		if (i == window_width/100 - 1 and not brick_live)
			break;
		
		while(user_sprite.getGlobalBounds().intersects(wall_sprite.getGlobalBounds())) { // если уже игрок находиться в стене, то толкаем вверх до тех пор, пока не будет не в стене
			user_y -= 1; // меняем положение игрока
			user_sprite.move(0, -1); // двигаем изобр. игрока наверх
			blocked_direction[2] = true;
		}	
		
		user_sprite.move(0, -user_speed); // двигаем изобр. игрока вниз, для проверки движения
		if(user_sprite.getGlobalBounds().intersects(wall_sprite.getGlobalBounds())) // проверка на наложение
			blocked_direction[0] = true; // блокируем движения игрока
		user_sprite.move(0, user_speed); // двигаем изобр. игрока наверх (на исходное)
		
		user_sprite.move(user_speed, 0); // двигаем изобр. игрока вправо, для проверки движения
		if(user_sprite.getGlobalBounds().intersects(wall_sprite.getGlobalBounds()))			
			blocked_direction[1] = true;
		user_sprite.move(-user_speed, 0); // двигаем изобр. игрока на исходное
		
		user_sprite.move(0, user_speed); // двигаем изобр. игрока вниз, для проверки движения
		if(user_sprite.getGlobalBounds().intersects(wall_sprite.getGlobalBounds()))
			blocked_direction[2] = true;
		user_sprite.move(0, -user_speed); // двигаем изобр. игрока на исходное
		
		user_sprite.move(-user_speed, 0); // двигаем изобр. игрока влево, для проверки движения
		if(user_sprite.getGlobalBounds().intersects(wall_sprite.getGlobalBounds()))
			blocked_direction[3] = true;
		user_sprite.move(user_speed, 0); // двигаем изобр. игрока на исходное
		
		window.draw(wall_sprite); // отрисока спрайта
		wall_sprite.move(100, 0); // двигаем от текущих коорд
	}
	
	if(brick_live){
		Texture bk_wall_texture; // создание текстуры
		bk_wall_texture.loadFromFile("source/breakable_brick.png"); // загрузить в текстуру картинку 
	 	
		Sprite bk_wall_sprite(bk_wall_texture); // загружаем в спрайт - текстуру (картинку) 
		bk_wall_sprite.setPosition(200, 400);
		window.draw(bk_wall_sprite);	
		
		RectangleShape hitpoint_1_wh(Vector2f(100, 25));  // Создаем прямоугольник размером 		
		hitpoint_1_wh.setPosition(200, 350);  //Устанавливаем его коорды  
		hitpoint_1_wh.setFillColor(Color::White);  //Устанавливаем ему цвет
		window.draw(hitpoint_1_wh);	 //Отрисовка прямоугольника
		
		Vertex hitpoint_1[] =
		{
			Vertex(Vector2f(200, 350)),  
			Vertex(Vector2f(200, 375)),  
			Vertex(Vector2f(200 + wall_health, 375)), 
			Vertex(Vector2f(200 + wall_health, 350))
		};
		
		hitpoint_1[3].color = Color(255, 0, 0);  //Устанавливаем цвет вершины
		hitpoint_1[2].color = Color(255, 0, 0);
		hitpoint_1[1].color = Color(128, 0, 0);  
		hitpoint_1[0].color = Color(128, 0, 0);
		
		window.draw(hitpoint_1, 4, Quads);  //Отрисовка хитпоинта
	}	
	
	window.draw(user_sprite); // отрисока игрока поверх всего
	
	RectangleShape hitpoint_2_wh(Vector2f(100, 25));  // Создаем прямоугольник размером 		
	hitpoint_2_wh.setPosition(user_x - 50, user_y - texture_size.y - 25 - 25);  //Устанавливаем его коорды 
	hitpoint_2_wh.setFillColor(Color::White);  //Устанавливаем ему цвет
	window.draw(hitpoint_2_wh);	 //Отрисовка прямоугольника
		
	Vertex hitpoint_2[] =
		{
			Vertex(Vector2f(user_x - 50, user_y - texture_size.y - 25 - 25)),  
			Vertex(Vector2f(user_x - 50, user_y - texture_size.y - 25 )),  
			Vertex(Vector2f(user_x - 50 + user_health, user_y - texture_size.y - 25 )), 
			Vertex(Vector2f(user_x - 50 + user_health, user_y - texture_size.y - 25 - 25))
		};
		
	hitpoint_2[3].color = Color(255, 0, 0);  //Устанавливаем цвет вершины
	hitpoint_2[2].color = Color(255, 0, 0);
	hitpoint_2[1].color = Color(128, 0, 0);  
	hitpoint_2[0].color = Color(128, 0, 0);
		
	window.draw(hitpoint_2, 4, Quads);  //Отрисовка хитпоинта
	
	if(wall_health < 0)
		brick_live = false;
}  

bool check_user_board(int x, int y){ // проверка на нажатие на модельку
	if(user_x - texture_size.x/2 > x or x > user_x + texture_size.x/2 or y > user_y or y < user_y - texture_size.y)
		return false; // если нажали за моделькой игрока
	return true;
}

void menu_render(){
	Texture wall_texture; // создание текстуры
	wall_texture.loadFromFile("source/brick.png"); // загрузить в текстуру картинку 
 	
	Sprite wall_sprite(wall_texture); // загружаем в спрайт - текстуру (картинку) 
	for(int i = 0; i < window_height/100; i++) // создаём фон 
		for(int j = 0; j < window_width/100; j++){
			wall_sprite.setPosition(j * 100, i * 100);
			window.draw(wall_sprite); //отрисока одного кирпича
			
			if((i+j) % 2 == 1){ // случайный разрушенный кирпич
				Texture bk_wall_texture; // создание текстуры
				bk_wall_texture.loadFromFile("source/breakable_brick.png"); // загрузить в текстуру картинку 
			 	
				Sprite bk_wall_sprite(bk_wall_texture); // загружаем в спрайт - текстуру (картинку) 
				bk_wall_sprite.setPosition(j * 100, i * 100);
				window.draw(bk_wall_sprite);
			}
		}

	RectangleShape white_button(Vector2f(400, 100));  // Создаем прямоугольник размером 		
	white_button.setPosition(400, 200);  //Устанавливаем его коорды 
	white_button.setFillColor(Color(255, 255, 255, 210));  //Устанавливаем ему цвет
	window.draw(white_button);	 //Отрисовка прямоугольника
	
	white_button.move(0, 300);  //Устанавливаем его коорды 
	window.draw(white_button);	 //Отрисовка прямоугольника

	Font font; //шрифт 
	if (!font.loadFromFile("source/Arkhip.ttf"))
	{
		cout<<"Error load font"<<endl;
	}
	else
	{
		font.loadFromFile("source/Arkhip.ttf"); // загрузка из файла
		Text text("Start game", font, 60); // настройка текста, размера, шрифта
		text.setColor(Color(90, 180, 0)); // цвет текста
		text.setPosition(420, 205); // устанавливаем корды
		window.draw(text); // отрисовка текста
		
		text.setString("Exit"); // установление текста
		text.setPosition(530, 505); // установка позиции
		window.draw(text); // отрисовка текста
	}
}

int main()
{		
	Clock clock; // создание времени
	
	SoundBuffer kickBuffer;//создаём буфер для звука
	kickBuffer.loadFromFile("source/8-bit-kick.wav");//загружаем в него звук
	Sound kick(kickBuffer);//создаем звук и загружаем в него звук из буфера
	kick.setVolume(50); // уменьшение громкости звука до 50%
	
	int temp_mass = mass; // восстановление массы
	
	while (window.isOpen()) // пока окно открыто 
	{
		
		if(scene == 0){ // если менб - то делаем начально игры (восстанавливаем)
			mass = 0;
			user_x = 600;
			user_y = 100; // коорды. игрока
			V_x = 0;
			V_y = 0; // вектора скорости
			face_user = "none"; // показывает куда направляется игрок
			click = false; // если произошло нажатие - true, иначе false
			interaction_user = false;
			brick_live = true;
			wall_health = 100; // здоровье стены
			user_health = 100; // здоровье игрока 
		}
		
		Event event; // обработчик событий
		
		if(face_user != "none" and clock.getElapsedTime().asMilliseconds() > 150) // обнуляем скорость, если прошло с обновлённого таймера более 500 мсек (нужно для плавного движения)
			V_x = 0; //заранее делаем скорость влево/вправо - нулевой
		while (window.pollEvent(event)) // проверка всех событий (в одном событие может быть несколько действий, например нажатие мыши и кнопки клавы)
		{
			if (event.type == Event::Closed) // если событие - закрыть окно
				window.close(); // закрываем окно
			if (event.type == sf::Event::KeyPressed){ // если нажали на клаву
			
                if(event.key.code == Keyboard::Escape){ // если клавиша - ESC
                	if(scene == 0){
                		update_menu = true;
                		mass = temp_mass; // переход в игру
						scene = 1;
                	}
                	else
                		scene = 0;
                }
                    
                if(event.key.code == sf::Keyboard::D){ // если клавиша - D
                	face_user = "right"; // направление взгляда игрока 
					V_x = user_speed; // добавляем скорость
					clock.restart(); // обнуляем таймер
                }        
				     
                if(event.key.code == sf::Keyboard::A){ // если клавиша - A
                	face_user = "left"; // направление взгляда игрока 
                	V_x = -user_speed; // добавляем скорость
                	clock.restart(); // обнуляем таймер
                }    
				    
                if(event.key.code == sf::Keyboard::E and user_x > 170 - texture_size.x/2 and user_x < 320 + texture_size.x/2 and brick_live){ // если клавиша - E
					wall_health--; // уменьшение жизни стены
					kick.play(); // проигрование звука
                }
                
				if(event.key.code == sf::Keyboard::W and (user_y == window_height or blocked_direction[2])) // если клавиша - W и если мы находимся не в воздухе
					V_y = -power_jump; // добавляем прыжок (отрицательная скорость в плоскости OY)
				
			}
			if (event.mouseButton.button == sf::Mouse::Left){ // если нажали мы ЛКМ	
				int x = event.mouseButton.x, y = event.mouseButton.y; // коорды текущего курсора 
				if (x >= 0 and x <= window_width and y >= 0 and y <= window_height){ //против неверных значений
					
					click = not click; // инверсия клика, отжали клавишу, или наоборот
					
					if(scene == 0 and click){
						if(400 <= x and x <= 800 and 200 <= y and y <= 300){ // Кнопка старта игры
							update_menu = true;
	                		mass = temp_mass; // переход в игру
							scene = 1;
						}
						
						if(400 <= x and x <= 800 and 500 <= y and y <= 600) // Кнопка выхода
							window.close();		
					}
				
					if(scene == 1){ // если поле игры
						if(not click) // Если отпустили клавишу
							interaction_user = false;
							
						if(click and check_user_board(x, y)){ // если нажата ЛКМ и нажали на игрока- то тогда вычисляем разницу между игроком и курсором, чтобы динамически сдвигать игрока
							delta_x = x - user_x;
								delta_y = y - user_y;
							interaction_user = true;
						}
					}
				}
			}
			if (interaction_user and event.type == sf::Event::MouseMoved){ // если всё ещё нажата ЛКМ, и мышь перемещаемся
				user_x = event.mouseMove.x - delta_x;
				user_y = event.mouseMove.y - delta_y; // устанавливаем новые коорды игрока
				check_board(); // проверка на границы
			}
		}
		window.clear(Color(55, 117, 176)); // заливаем экран в голубой
		
		if(scene == 1){		
			Update(); // процедура на сдвиг и "физику"
			Render(); // процедура на отрисовку объектов
		}
		
		if(scene == 0)
			menu_render();
		
		//cout<<scene<<endl;
		
		window.display();  //Отрисовка окна/кадра
	}
	return 0;
}