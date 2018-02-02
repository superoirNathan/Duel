//Nathan Alphonse 100523109

#pragma once

#include <vector>
#include "animationMath.h"
template <typename T>
struct SpeedControlTableRow
{
    int segment;
    float tValue;
    float arcLength;
    float arcLengthNormalized;
    T sampleValue;
};

template <typename T>
class KeyframeController
{
private:
    unsigned int m_pCurrentKeyframe;
    unsigned int m_pNextKeyframe;
    std::vector<T> m_pKeys;
    float m_pKeyLocalTime;

    std::vector< SpeedControlTableRow<T> > m_pSpeedControlLookUpTable;

public:
    KeyframeController()
        : m_pCurrentKeyframe(0),
        m_pNextKeyframe(1),
        m_pKeyLocalTime(0.0f),
        paused(false),
        loops(true),
        doesSpeedControl(false),
        curvePlaySpeed(1.0f)
    {

    }
    // Calulate Blue Bar
    void calculateLookupTable(int numSamples)
    {
        if (m_pKeys.size() > 1) // Make sure we have enough points
        {
            m_pSpeedControlLookUpTable.clear(); // clear existing table

            float timeStep = 1.0f / numSamples; // This controls the interval at which we will sample the segment

                                                // Create table and compute segment, t value and sample columns of table
            if (m_pKeys.size() >= 3) {
                for (unsigned int i = 0; i < m_pKeys.size() - (1); i++) // loop through each segment
                {
                    for (float j = 0.0f; j <= 1.0f; j += timeStep) // iterate through each sample on the current segment
                    {
                        // Create a new SpeedControlTableRow and fill it in with the appropriate data
                        SpeedControlTableRow<T> row;

                        row.segment = i; // number of points to go through along a path path
                        row.tValue = j; // light blue value

                                        //					row.sampleValue = Math::lerp(m_pKeys[i], m_pKeys[i + 1], j); // For this lab, we'll use lerp. But this exact algorithm works for catmull or bezier too.
                        if (i == 0)
                        {
                            row.sampleValue = Math::catmull(m_pKeys[i], m_pKeys[i], m_pKeys[i + 1], m_pKeys[i + 2], j); // For this lab, we'll use lerp. But this exact algorithm works for catmull or bezier too.
                        }
                        else if (i == m_pKeys.size() - (2)) {
                            row.sampleValue = Math::catmull(m_pKeys[i - 1], m_pKeys[i], m_pKeys[i + 1], m_pKeys[i + 1], j); // For this lab, we'll use lerp. But this exact algorithm works for catmull or bezier too.
                        }
                        else {
                            row.sampleValue = Math::catmull(m_pKeys[i - 1], m_pKeys[i], m_pKeys[i + 1], m_pKeys[i + 2], j); // For this lab, we'll use lerp. But this exact algorithm works for catmull or bezier too.
                        }

                        m_pSpeedControlLookUpTable.push_back(row);
                    }
                }
            }
            else {
                for (unsigned int i = 0; i < m_pKeys.size() - (1); i++) // loop through each segment
                {
                    for (float j = 0.0f; j <= 1.0f; j += timeStep) // iterate through each sample on the current segment
                    {
                        // Create a new SpeedControlTableRow and fill it in with the appropriate data
                        SpeedControlTableRow<T> row;

                        row.segment = i; // number of points to go through along a path path
                        row.tValue = j; // light blue value

                        row.sampleValue = Math::lerp(m_pKeys[i], m_pKeys[i + 1], j); // For this lab, we'll use lerp. But this exact algorithm works for catmull or bezier too.

                        m_pSpeedControlLookUpTable.push_back(row);
                    }
                }
            }

            //Check if we have blue bars
            // Calculate arc length column of table
            int numEntries = m_pSpeedControlLookUpTable.size();

            if (numEntries == 0) // if you did the above loop correctly, this shouldnt happen
            {
                std::cout << "Failed to create look up table. " << std::endl;
                return;
            }

            // Initialize first row of table
            // Remember the struct has no constuctor so we need to make sure to set everything manually
            // Note: the slides refer "arcLength" as "distance on curve"
            m_pSpeedControlLookUpTable[0].arcLength = 0.0f;
            m_pSpeedControlLookUpTable[0].arcLengthNormalized = 0.0f;

            //For every u value
            // Loop through each point in the table and calculate the distance from the beginning of the path
            for (int i = 1; i < numEntries; i++)
            {
                //Find green bar
                // distance = length(current sample value - previous sample value)
                T  distance = (m_pSpeedControlLookUpTable[i].sampleValue - m_pSpeedControlLookUpTable[i - 1].sampleValue);
                float D = distance.x*distance.x + distance.y*distance.y + distance.z*distance.z;
                D = sqrt(D);
                //Find blue bar
                m_pSpeedControlLookUpTable[i].arcLength = D + m_pSpeedControlLookUpTable[i - 1].arcLength;
            }

            // Normalize the curve
            // This means 0 will be at the start of the path, and 1 will be at the end of the entire path
            float totalCurveLength = m_pSpeedControlLookUpTable[numEntries - 1].arcLength; // length of the path = distance the last sample is from the beginning

                                                                                           // Normalize each sample
                                                                                           // Loop through each entry in the table
                                                                                           // Set "ArcLengthNormalized" to sample's distance on curve divided by total length of curve
            for (int i = 1; i < numEntries; i++)
            {
                m_pSpeedControlLookUpTable[i].arcLengthNormalized = m_pSpeedControlLookUpTable[i].arcLength / totalCurveLength;

            }
        }
    }

    T speedControlledUpdate(float dt)
    {
        // key local time is now from start of curve to end of curve, instead of per segment

        ///v = d/t -> d = v*t
        // as speed increases, less frames/updates (dt) needed to reach time
        // as speed decreases, more frames/updates (dt) needed to reach time

        // the blue value of our point of interest ( u or % or distance)
        m_pKeyLocalTime += (dt / curvePlaySpeed); // control playback speed ///might be (*) instead of (/)

        updateSegmentIndices();

        // look up values from table

        // for each u value
        // Loop through each row in the table
        for (unsigned int i = 1; i < m_pSpeedControlLookUpTable.size(); i++)
        {

            // if its time to use next blue value
            // Find the first sample who's distance is >= m_pKeyLocalTime
            if (m_pSpeedControlLookUpTable[i].arcLengthNormalized >= m_pKeyLocalTime)
            {
                // calculate p0 & p1, then use t inbetween points to get u inbetween points of our blue values
                // then use u points and curve points to solve for the red points

                // float arc0 = previous sample's normalized distance
                float arc0 = m_pSpeedControlLookUpTable[i - 1].arcLengthNormalized;
                //float arc1 = m_pSpeedControlLookUpTable[i].arcLengthNormalized; // current sample's normalized distance
                float arc1 = m_pSpeedControlLookUpTable[i].arcLengthNormalized;

                //rearrange lerp formula and solve for t using 2 blue points and current u value

                //float tVal = Math::invLerp(m_pKeyLocalTime, arc0, arc1); // "inverse lerp" i.e. given 3 points, solve the tValue
                float tVal = Math::invLerp(m_pKeyLocalTime, arc0, arc1);

                // calculate intermediate table
                // T sample0 = previous sample value
                T sample0 = m_pSpeedControlLookUpTable[i - 1].sampleValue;
                // T sample1 = current sample value
                T sample1 = m_pSpeedControlLookUpTable[i].sampleValue;

                // use our 2 red values relative to our 2 blue values to get our inbetween red value using our inbetweent t value we solved for
                return Math::lerp(sample0, sample1, tVal);
            }
        }

        return T(); // if lookup table is set up correctly, this should never trigger
    }

    T update(float dt)
    {
        if (m_pKeys.size() < 2)
            return T();

        if (doesSpeedControl)
            return speedControlledUpdate(dt);
        else
            return noSpeedControlUpdate(dt);
    }

    void updateSegmentIndices()
    {
        if (m_pKeyLocalTime >= 1.0f)
        {
            m_pKeyLocalTime = 0.0f;
            m_pCurrentKeyframe++;
            m_pNextKeyframe++;

            // If we're at the end of the animation, jump back to beginning
            // Note: you can add additional logic here to handle end of animation behaviour
            // such as: ping-ponging (playing in reverse back to beginning), closed loop, etc.
            if (m_pNextKeyframe >= m_pKeys.size())
            {
                m_pCurrentKeyframe = 0;
                m_pNextKeyframe = 1;
            }
        }
    }
    T noSpeedControlUpdate(float dt)
    {
        m_pKeyLocalTime += dt;

        updateSegmentIndices();

        T p0 = m_pKeys[m_pCurrentKeyframe];
        T p1 = m_pKeys[m_pNextKeyframe];

        return Math::lerp<T>(p0, p1, m_pKeyLocalTime);
    }

    void addKey(T key)
    {
        m_pKeys.push_back(key);
    }

    void setKey(unsigned int idx, T key)
    {
        if (idx >= m_pKeys.size())
            return;
        m_pKeys[idx] = key;
    }

    float curvePlaySpeed;
    bool paused;
    bool loops;
    bool doesSpeedControl;
};