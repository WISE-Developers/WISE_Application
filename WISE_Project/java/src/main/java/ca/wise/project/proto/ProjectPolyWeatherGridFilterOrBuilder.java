// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: projectGridFilter.proto

package ca.wise.project.proto;

@java.lang.Deprecated public interface ProjectPolyWeatherGridFilterOrBuilder extends
    // @@protoc_insertion_point(interface_extends:WISE.ProjectProto.ProjectPolyWeatherGridFilter)
    com.google.protobuf.MessageOrBuilder {

  /**
   * <code>int32 version = 1;</code>
   * @return The version.
   */
  int getVersion();

  /**
   * <code>string name = 2;</code>
   * @return The name.
   */
  java.lang.String getName();
  /**
   * <code>string name = 2;</code>
   * @return The bytes for name.
   */
  com.google.protobuf.ByteString
      getNameBytes();

  /**
   * <code>string comments = 3;</code>
   * @return The comments.
   */
  java.lang.String getComments();
  /**
   * <code>string comments = 3;</code>
   * @return The bytes for comments.
   */
  com.google.protobuf.ByteString
      getCommentsBytes();

  /**
   * <code>.google.protobuf.UInt32Value color = 4;</code>
   * @return Whether the color field is set.
   */
  boolean hasColor();
  /**
   * <code>.google.protobuf.UInt32Value color = 4;</code>
   * @return The color.
   */
  com.google.protobuf.UInt32Value getColor();
  /**
   * <code>.google.protobuf.UInt32Value color = 4;</code>
   */
  com.google.protobuf.UInt32ValueOrBuilder getColorOrBuilder();

  /**
   * <code>.google.protobuf.UInt64Value size = 5;</code>
   * @return Whether the size field is set.
   */
  boolean hasSize();
  /**
   * <code>.google.protobuf.UInt64Value size = 5;</code>
   * @return The size.
   */
  com.google.protobuf.UInt64Value getSize();
  /**
   * <code>.google.protobuf.UInt64Value size = 5;</code>
   */
  com.google.protobuf.UInt64ValueOrBuilder getSizeOrBuilder();

  /**
   * <code>bool landscape = 6;</code>
   * @return The landscape.
   */
  boolean getLandscape();

  /**
   * <code>.WISE.WeatherProto.WeatherGridFilter filter = 7;</code>
   * @return Whether the filter field is set.
   */
  boolean hasFilter();
  /**
   * <code>.WISE.WeatherProto.WeatherGridFilter filter = 7;</code>
   * @return The filter.
   */
  ca.wise.weather.proto.WeatherGridFilter getFilter();
  /**
   * <code>.WISE.WeatherProto.WeatherGridFilter filter = 7;</code>
   */
  ca.wise.weather.proto.WeatherGridFilterOrBuilder getFilterOrBuilder();
}