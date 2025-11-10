package com.quforia

class TrackingResult(
    private val name: String,
    private val poseMatrix: FloatArray,
    private val status: Int
) {
    fun getName(): String = name
    fun getPoseMatrix(): FloatArray = poseMatrix
    fun getStatus(): Int = status

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (javaClass != other?.javaClass) return false
        other as TrackingResult
        if (name != other.name) return false
        if (!poseMatrix.contentEquals(other.poseMatrix)) return false
        if (status != other.status) return false
        return true
    }

    override fun hashCode(): Int {
        var result = name.hashCode()
        result = 31 * result + poseMatrix.contentHashCode()
        result = 31 * result + status
        return result
    }
}
