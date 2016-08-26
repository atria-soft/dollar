/** @file
 * @author Edouard DUPIN
 * @copyright 2016, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once
#include <etk/types.h>
#include <etk/math/Vector2D.h>
#include <dollar/Rectangle.h>

namespace dollar {
	/**
	 * @brief Get the size of a path (not the number of element but the distance of the path
	 * @param[in] _points List of point of the path
	 * @return the size of the path
	 */
	float pathLength(std::vector<vec2> _points);
	/**
	 * @brief Get the center position of the Path (baricenter)...
	 * @param[in] _points List of points.
	 * @return The center position of all the points.
	 */
	vec2 getBaryCenter(const std::vector<vec2>& _points);
	/**
	 * @brief Rotate a list of point from the center to the precise angle.
	 * @param[in] _points List of point in the path.
	 * @param[in] _rotation Angle to apply.
	 * @return new path with the rotated points.
	 */
	// TODO: Change this with the use of a generic matrix 2D...
	std::vector<vec2> rotateBy(const std::vector<vec2>& _points, float _rotation);
	/**
	 * @brief Rotate a path to a specific angle (0rad)
	 * @param[in] _points Path to rotate
	 * @return the Path aligned with 0;
	 */
	std::vector<vec2> rotateToZero(const std::vector<vec2>& _points);
	/**
	 * @brief Get the Bounding box associated at a path
	 * @param[in] _points List of point at the path
	 * @return the rectangle with the bounding box associated
	 */
	dollar::Rectangle boundingBox(const std::vector<vec2>& _points);
	/**
	 * @brief Scale the list of point in a 1.0*1.0 box started at 0.0*0.0
	 * @param[in] _points input path
	 * @param[in] _keepAspectRation Keep the aspect ratio of the scaling
	 * @return modify points
	 */
	std::vector<vec2> scaleToOne(const std::vector<vec2>& _points, bool _keepAspectRation=true);
	/**
	 * @brief Scale the list of point in a 1.0*1.0 box started at 0.0*0.0
	 * @param[in] _points input path
	 * @param[in] _keepAspectRation Keep the aspect ratio of the scaling
	 * @return modify points
	 */
	std::vector<std::vector<vec2>> scaleToOne(const std::vector<std::vector<vec2>>& _points, bool _keepAspectRation=false);
	/**
	 * @brief Get center of the path and move the path to be center at (0,0)
	 * @param[in] _points List of point in the path
	 * @return centered path.
	 */
	std::vector<vec2> translateBariCenterToZero(std::vector<vec2> _points);
	/**
	 * @brief Resample a path With a specific number of elements
	 * @param[in] _points Path to change number of elements
	 * @param[in] _nbPoints Number of point desired in the output path
	 * @return Resample path.
	 */
	std::vector<vec2> resample(std::vector<vec2> _points, int32_t _nbPoints);
	/**
	 * @brief Make a list of all single stroke possible for a specific stroke (take in case the user can write the second stroke befor the first and he can do the stroke in the oposite way
	 * @param[in] _strokes List of all strokes
	 * @return List of a list of single stroke of multiple stroke
	 */
	std::vector<std::vector<vec2>> makeReferenceStrokes(const std::vector<std::vector<vec2>>& _strokes);
	/**
	 * @brief combine some stroke in a single one.
	 * @param[in] _strokes value to merge
	 * @return Merged vector
	 */
	std::vector<vec2> combineStrokes(const std::vector<std::vector<vec2>>& _strokes);
	/**
	 * @brief Normalise the Path with the full magnetude of the data
	 * @param[in] _points Input path
	 * @return Normalized path
	 */
	std::vector<vec2> normalyse(const std::vector<vec2>& _points);
	/**
	 * @brief Calculate a starting vector of the path
	 * @param[in] _points List of point of the path
	 * @param[in] _index position index to get the vector (ratio to 0)
	 * @return the start vector.
	 */
	vec2 getStartVector(const std::vector<vec2>& _points, float _index = 1);
	/**
	 * @brief Transform the path to be comparable, resample the path with a specific number of sample, and limit size at 1.0 square center around 0
	 * @param[in] _points List of points in the path
	 * @param[in] _nbSample Number of element to resample
	 * @param[in] _ignoreRotation Ignore start rotation of the algorithm
	 * @param[in] _keepAspectRatio Keep Aspect ratio when scaling to the correct size (1.0,1.0) (it will be centered)
	 * @return new list of points
	 */
	std::vector<vec2> normalizePath(std::vector<vec2> _points, size_t _nbSample, bool _ignoreRotation, bool _keepAspectRatio);
	
	/**
	 * @brief Transform the path to be comparable, resample the path with a specific number of sample, and limit size at 1.0 square center around 0
	 * @note The difference with @ref normalizePath is thet we do not combinethe path together, that permit to not have unneded point between strokes...
	 * @param[in] _points List of points in the path
	 * @param[in] _distance Distance between points
	 * @param[in] _keepAspectRatio Keep Aspect ratio when scaling to the correct size (1.0,1.0) (it will be centered)
	 * @return new list of points
	 */
	std::vector<vec2> normalizePathToPoints(std::vector<std::vector<vec2>> _points, float _distance, bool _keepAspectRatio);
}


