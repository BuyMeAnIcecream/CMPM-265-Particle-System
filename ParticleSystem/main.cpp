#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Main.hpp>


class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:
	sf::Texture tex;
	sf::Color col;
	ParticleSystem(unsigned int count, float ac, float sc, sf::Color c) :
		m_particles(count),
		m_vertices(sf::Triangles, 3 * count),
		m_lifetime(sf::seconds(3)),
		m_emitter(0, 0),
		size(count),
		angleCoef(ac),
		speedCoef(sc),
		col(c)
		
	{
		tex.loadFromFile("triangle.png");
	}
	int size;
	float angleCoef;
	float speedCoef;
	void resizeParticlesArr(unsigned int count){
		m_particles.resize(count);
		m_vertices.resize(count*3);
		size = count;
		

	}
	void setEmitter(sf::Vector2f position)
	{
		m_emitter = position;
	}

	void update(sf::Time elapsed)
	{
		for (std::size_t i = 0; i < m_particles.size(); ++i)
		{
			// update the particle lifetime
			Particle& p = m_particles[i];
			p.lifetime -= elapsed;

			// if the particle is dead, respawn it
			if (p.lifetime <= sf::Time::Zero)
				resetParticle(i);
			p.velocity.y += p.gravityCoef;
			// update the position of the corresponding vertex
			m_vertices[i*3].position += p.velocity * elapsed.asSeconds();
			m_vertices[i*3+1].position += p.velocity * elapsed.asSeconds();
			m_vertices[i*3+2].position += p.velocity * elapsed.asSeconds();


			// update the alpha (transparency) of the particle according to its lifetime
			float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
			
			m_vertices[i*3].color.a = static_cast<sf::Uint8>(ratio * 255);
			m_vertices[i * 3+1].color.a = static_cast<sf::Uint8>(ratio * 255);
			m_vertices[i * 3+2].color.a = static_cast<sf::Uint8>(ratio * 255);
		}
	}

private:

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
	{
		// apply the transform
		states.transform *= getTransform();

		// our particles don't use a texture
		states.texture =  &tex;
		
		// draw the vertex array
		target.draw(m_vertices, states);
	
	}

private:         
	 

	struct Particle
	{
		sf::Vector2f velocity;
		sf::Time lifetime;
		float gravityCoef = 0.1;  
		  
	};

	void resetParticle(std::size_t index)
	{
		// give a random velocity and lifetime to the particle
		float angle = (std::rand() % 360 / angleCoef) * 3.14f / 180.f;
		float speed = (std::rand() % 50 / speedCoef) + 50.f;
		m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
		m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

		// reset the position of the corresponding vertex
		m_vertices[index * 3].position = m_emitter + sf::Vector2f(-20, 0);
		m_vertices[index * 3+1].position = m_emitter + sf::Vector2f(0, -20);
		m_vertices[index * 3+2].position = m_emitter + sf::Vector2f(20, 0);
		
		m_vertices[index * 3].color = col;
		m_vertices[index * 3 + 1].color = col;
		m_vertices[index * 3 + 2].color = col;

		m_vertices[index * 3].texCoords = m_emitter + sf::Vector2f(0, 466);
		m_vertices[index * 3 + 1].texCoords = m_emitter + sf::Vector2f(250, 0);
		m_vertices[index * 3 + 2].texCoords = m_emitter + sf::Vector2f(499, 446);

	}

	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	sf::Time m_lifetime;
	sf::Vector2f m_emitter;
};

int main()
{
	// create the window
	sf::RenderWindow window(sf::VideoMode(512, 256), "Particles");
    
	// create the particle system
	ParticleSystem particles(500,3,2,sf::Color(140,121,12));
	ParticleSystem particles2(2000,2,4, sf::Color(45, 32, 100));
	ParticleSystem particles3(1000,5,2, sf::Color(23, 200, 15));

	// create a clock to track the elapsed time
	sf::Clock clock;
	particles.setEmitter(window.mapPixelToCoords(sf::Vector2i(512/2,256/2)));
	particles2.setEmitter(window.mapPixelToCoords(sf::Vector2i(512 / 4, 256 / 4)));
	particles3.setEmitter(window.mapPixelToCoords(sf::Vector2i(512 / 6, 256 / 2)));
	// run the main loop
	while (window.isOpen())
	{
		
		// handle events
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				particles.resizeParticlesArr(particles.size *2);
				particles2.resizeParticlesArr(particles.size * 2);
				particles3.resizeParticlesArr(particles.size * 2);
				// left key is pressed: move our character

			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				particles.resizeParticlesArr(particles.size / 2);
				particles2.resizeParticlesArr(particles.size / 2);
				particles3.resizeParticlesArr(particles.size / 2);

				// left key is pressed: move our character

			}
		}

		// make the particle system emitter follow the mouse
//		sf::Vector2i mouse = sf::Mouse::getPosition(window);
//		particles.setEmitter(window.mapPixelToCoords(mouse));

		// update it
		sf::Time elapsed = clock.restart();
		particles.update(elapsed);
		particles2.update(elapsed);
		particles3.update(elapsed);
		// draw it
		window.clear();
		window.draw(particles);
		window.draw(particles2);
		window.draw(particles3);
		window.display();
	}

	return 0;
}