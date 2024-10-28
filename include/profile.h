#pragma once

#include <Arduino.h>
#include "encoders.h"
#include "config.h"

class Profile;

extern Profile forward;
extern Profile rotation;

class Profile
{
public:
    enum State : uint8_t
    {
        PS_IDLE = 0,
        PS_ACCELERATING = 1,
        PS_BRAKING = 2,
        PS_FINISHED = 3,
    };

    void reset()
    {
        noInterrupts();
        m_position = 0;
        m_speed = 0;
        m_target_speed = 0;
        m_state = PS_IDLE;
        interrupts();
    }

    bool is_finished()
    {
        return m_state == PS_FINISHED;
    }

    void start(float distance, float top_speed, float final_speed, float acceleration)
    {
        m_sign = (distance < 0) ? -1 : +1;
        if (distance < 0)
        {
            distance = -distance;
        }
        if (distance < 1.0)
        {
            m_state = PS_FINISHED;
            return;
        }
        if (final_speed > top_speed)
        {
            final_speed = top_speed;
        }

        m_position = 0;
        m_final_position = distance;
        m_target_speed = m_sign * fabsf(top_speed);
        m_final_speed = m_sign * fabsf(final_speed);
        m_acceleration = fabsf(acceleration);
        if (m_acceleration >= 1)
        {
            m_one_over_acc = 1.0f / m_acceleration;
        }
        else
        {
            m_one_over_acc = 1.0;
        }
        m_state = PS_ACCELERATING;
    }


    void stop()
    {
        noInterrupts();
        m_target_speed = 0;
        noInterrupts();
        finish();
    }

    void finish()
    {
        noInterrupts();
        m_speed = m_target_speed;
        m_state = PS_FINISHED;
        interrupts();
    }

    void wait_until_finished()
    {   
        
        while (m_state != PS_FINISHED)
        {
            delay(2);
        }
    }

    void set_state(State state)
    {
        m_state = state;
    }

    float get_braking_distance()
    {
        return fabsf(m_speed * m_speed - m_final_speed * m_final_speed) * 0.5 * m_one_over_acc;
    }

    float position()
    {
        float pos;

        noInterrupts();
        pos = m_position;
        interrupts();
        return pos;
    }

    float speed()
    {
        float speed;
        noInterrupts();
        speed = m_speed;
        interrupts();
        return speed;
    }

    float acceleration()
    {
        float acc;
        noInterrupts();
        acc = m_acceleration;
        interrupts();
        return acc;
    }

    void set_speed(float speed)
    {
        noInterrupts();
        m_speed = speed;
        interrupts();
    }

    void set_target_speed(float speed)
    {
        noInterrupts();
        m_target_speed = speed;
        interrupts();
    }

    // only used to alter position for forward error correction
    void adjust_position(float adjustment)
    {
        noInterrupts();
        m_position += adjustment;
        interrupts();
    }

    void set_position(float position)
    {
        noInterrupts();
        m_position = position;
        interrupts();
    }

    void update()
    {
        if (m_state == PS_IDLE)
        {
            return;
        }
        float delta_v = m_acceleration * encoders.getLoopTime();
        float remaining = fabsf(m_final_position) - fabsf(m_position);
        if (m_state == PS_ACCELERATING)
        {
            if (remaining < get_braking_distance())
            {
                m_state = PS_BRAKING;
                if (m_final_speed == 0)
                {
                    m_target_speed = m_sign * 30.0f; // magic number to make sure we reach zero
                }
                else
                {
                    m_target_speed = m_final_speed;
                };
            }
        }
        
        // try to reach the target speed
        if (m_speed < m_target_speed)
        {
            m_speed += delta_v;
            if (m_speed > m_target_speed)
            {
                m_speed = m_target_speed;
            }
        }
        if (m_speed > m_target_speed)
        {
            m_speed -= delta_v;
            if (m_speed < m_target_speed)
            {
                m_speed = m_target_speed;
            }
        }
        // increment the position
        m_position += m_speed * encoders.getLoopTime();
        if (m_state != PS_FINISHED && remaining < 0.125)
        {
            m_state = PS_FINISHED;
            m_target_speed = m_final_speed;
        }
    }

private:
    volatile uint8_t m_state = PS_IDLE;
    volatile float m_speed = 0;
    volatile float m_position = 0;
    int8_t m_sign = 1;
    float m_acceleration = 0;
    float m_one_over_acc = 1;
    float m_target_speed = 0;
    float m_final_speed = 0;
    float m_final_position = 0;
};
