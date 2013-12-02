
#include "pendulumSystem.h"
#include <iostream>

PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles)
{
	mass = 0.05f;
    gravity = Vector3f(0.0f, -1.0f, 0.0f);
    drag_const = 0.01f;
    spring_const = 4.0f;
    rest_len = 0.5f;

	m_numParticles = numParticles;
	drawSprings = false;
	
	// fill in code for initializing the state based on the number of particles
	for (int i = 0; i < m_numParticles; i++) {
		// ACTUAL HAIR PARTICLE
		// position vector
		if (i == 0) {
			m_vVecState.push_back(Vector3f(0,0,0));
		}

		else {
			m_vVecState.push_back(Vector3f(i*rest_len, 0, 0));
		}

		// velocity vector
		m_vVecState.push_back(Vector3f(0,0,0));


		// GHOST PARTICLES (to form triangles)
		// position vector
		if (i == 0) {
			continue;
		}
		else {
			ghost_particles.push_back(Vector3f((i-0.5)*rest_len, 0.2f, 0));
		}

		// velocity vector
		ghost_particles.push_back(Vector3f(0,0,0));


		// SPRINGS
		// edge springs
		vector<Vector3f> edge;
		if ((i-1) > -1) {
			edge.push_back(Vector3f(i-1, spring_const, rest_len)); // took out multiplying spring_const*i?
		}

		if ((i+1) < m_numParticles) {
			edge.push_back(Vector3f(i+1, spring_const, rest_len)); // same here
		}

		edge_springs.push_back(edge);
		
		vector<Vector3f> ghost_edge;
		// first value is index of hair particle that ghost particle i should have an edge with
		if (i == 0) {
			ghost_edge.push_back(Vector3f(i+1, spring_const, 0.2f));
		}
		else if (i == m_numParticles-1) {
			ghost_edge.push_back(Vector3f(i-1, spring_const, 0.2f));
		}
		else {
			ghost_edge.push_back(Vector3f(i+1, spring_const, 0.2f));
			ghost_edge.push_back(Vector3f(i-1, spring_const, 0.2f));
		}

		ghost_edge_springs.push_back(ghost_edge);


		// bending springs
		vector<Vector3f> bend;
		if ((i-2) > -1) {
			bend.push_back(Vector3f(i-2, spring_const, 2*rest_len));
		}
		if ((i+2) < m_numParticles) {
			bend.push_back(Vector3f(i+2, spring_const, 2*rest_len));
		}

		bend_springs.push_back(bend);

		vector<Vector3f> ghost_bend;
		// first value is index of ghost particle that ghost particle i should have an edge with
		if ((i-1) > -1) {
			ghost_bend.push_back(Vector3f(i-1, spring_const, rest_len));
		}
		if ((i+1) < m_numParticles) {
			ghost_bend.push_back(Vector3f(i+1, spring_const, rest_len));
		}

		// torsion springs
		vector<Vector3f> torsion;
		float hypotenuse = 0; // DO THISSSS

		// first value is index of ghost particle that hair particle i should have an edge with
		if ((i+1) > (m_numParticles-1)) {
			torsion.push_back(Vector3f(i+1, spring_const, hypotenuse));
		}
		else {
			torsion.push_back(Vector3f(i-1, spring_const, hypotenuse))
		}
	}
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	for (int i=0; i < m_numParticles; i++) {
		Vector3f current_position = getParticlePosition(state, i);
		Vector3f current_velocity = getParticleVelocity(state, i);

		Vector3f force;
		if (i == 0) {
			force = Vector3f(0,0,0);
		}

		else {
			force = (mass*gravity) - drag_const*current_velocity;

			vector<Vector3f> spr = springs[i];
			for (int j=0; j < spr.size(); j++) {
				Vector3f disp = current_position - getParticlePosition(state, spr[j][0]);
				force += -spr[j][1]*(disp.abs() - spr[j][2])*disp/disp.abs();
			}
		}

		f.push_back(current_velocity);
		f.push_back(force/mass);
	}

	return f;
}

Vector3f PendulumSystem::getParticlePosition(vector<Vector3f> state, int x) {
	return state[2*x];
}

Vector3f PendulumSystem::getParticleVelocity(vector<Vector3f> state, int x) {
	return state[2*x + 1];
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos = m_vVecState[2*i] ;//  position of particle i. YOUR CODE HERE
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.075f,10.0f,10.0f);
        
        //drawing cylinders at each particle point
        if (i+1<m_numParticles){
            Vector3f pos2 = m_vVecState[2*(i+1)];
            //glRotatef(90.0f,1.0f,0,0);
            float angleRad = atanf((pos[0]-pos2[0])/(pos[1]-pos2[1]));
            float angleDeg = angleRad * 180 / 3.1415296;
            glRotatef(-angleDeg,0,0,1.0f);
            glRotatef(90.0f,1.0f,0.0f,0.0f); //switch y and z axis
            //glRotatef(0.0f,pos2[0],pos2[1],pos2[2]);
            GLUquadricObj *quad= gluNewQuadric();
            gluCylinder(quad,0.075f,0.075f,0.5f,32,32); }
        glPopMatrix();
        
		glPopMatrix();
	}
    

	if (drawSprings) {
		for(int i=0; i < springs.size(); i++) {
	    	vector<Vector3f> s = springs[i];

			for (int j=0; j < s.size(); j++) {
				Vector3f pos1 = m_vVecState[2*i];
				Vector3f pos2 = m_vVecState[2*s[j][0]];

				glDisable(GL_LIGHTING);
				glBegin(GL_LINES);
				glColor3f(1.0,1.0,1.0);
				glVertex3f(pos1[0], pos1[1], pos1[2]);
				glVertex3f(pos2[0], pos2[1], pos2[2]);
				glEnd();
				glEnable(GL_LIGHTING);
                
			}
		}
	}
}

void PendulumSystem::showSprings() {
	if (drawSprings) {
		drawSprings = false;
	}

	else {
		drawSprings = true;
	}
}