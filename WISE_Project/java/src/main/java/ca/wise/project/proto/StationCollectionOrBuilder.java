// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: weatherStation.proto

package ca.wise.project.proto;

public interface StationCollectionOrBuilder extends
    // @@protoc_insertion_point(interface_extends:WISE.ProjectProto.StationCollection)
    com.google.protobuf.MessageOrBuilder {

  /**
   * <code>int32 version = 1;</code>
   * @return The version.
   */
  int getVersion();

  /**
   * <pre>
   * [deprecated = true] this is really deprecated but can't or the code we need to generate to save an old file format isn't generated
   * </pre>
   *
   * <code>repeated .WISE.ProjectProto.ProjectWeatherStation stations = 2;</code>
   */
  java.util.List<ca.wise.project.proto.ProjectWeatherStation> 
      getStationsList();
  /**
   * <pre>
   * [deprecated = true] this is really deprecated but can't or the code we need to generate to save an old file format isn't generated
   * </pre>
   *
   * <code>repeated .WISE.ProjectProto.ProjectWeatherStation stations = 2;</code>
   */
  ca.wise.project.proto.ProjectWeatherStation getStations(int index);
  /**
   * <pre>
   * [deprecated = true] this is really deprecated but can't or the code we need to generate to save an old file format isn't generated
   * </pre>
   *
   * <code>repeated .WISE.ProjectProto.ProjectWeatherStation stations = 2;</code>
   */
  int getStationsCount();
  /**
   * <pre>
   * [deprecated = true] this is really deprecated but can't or the code we need to generate to save an old file format isn't generated
   * </pre>
   *
   * <code>repeated .WISE.ProjectProto.ProjectWeatherStation stations = 2;</code>
   */
  java.util.List<? extends ca.wise.project.proto.ProjectWeatherStationOrBuilder> 
      getStationsOrBuilderList();
  /**
   * <pre>
   * [deprecated = true] this is really deprecated but can't or the code we need to generate to save an old file format isn't generated
   * </pre>
   *
   * <code>repeated .WISE.ProjectProto.ProjectWeatherStation stations = 2;</code>
   */
  ca.wise.project.proto.ProjectWeatherStationOrBuilder getStationsOrBuilder(
      int index);

  /**
   * <code>repeated .WISE.WeatherProto.CwfgmWeatherStation wxStationData = 3;</code>
   */
  java.util.List<ca.wise.weather.proto.CwfgmWeatherStation> 
      getWxStationDataList();
  /**
   * <code>repeated .WISE.WeatherProto.CwfgmWeatherStation wxStationData = 3;</code>
   */
  ca.wise.weather.proto.CwfgmWeatherStation getWxStationData(int index);
  /**
   * <code>repeated .WISE.WeatherProto.CwfgmWeatherStation wxStationData = 3;</code>
   */
  int getWxStationDataCount();
  /**
   * <code>repeated .WISE.WeatherProto.CwfgmWeatherStation wxStationData = 3;</code>
   */
  java.util.List<? extends ca.wise.weather.proto.CwfgmWeatherStationOrBuilder> 
      getWxStationDataOrBuilderList();
  /**
   * <code>repeated .WISE.WeatherProto.CwfgmWeatherStation wxStationData = 3;</code>
   */
  ca.wise.weather.proto.CwfgmWeatherStationOrBuilder getWxStationDataOrBuilder(
      int index);
}
