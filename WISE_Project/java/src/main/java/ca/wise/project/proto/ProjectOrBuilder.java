// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: cwfgmProject.proto

package ca.wise.project.proto;

public interface ProjectOrBuilder extends
    // @@protoc_insertion_point(interface_extends:WISE.ProjectProto.Project)
    com.google.protobuf.MessageOrBuilder {

  /**
   * <code>int32 version = 1;</code>
   * @return The version.
   */
  int getVersion();

  /**
   * <code>string comments = 2;</code>
   * @return The comments.
   */
  java.lang.String getComments();
  /**
   * <code>string comments = 2;</code>
   * @return The bytes for comments.
   */
  com.google.protobuf.ByteString
      getCommentsBytes();

  /**
   * <code>.WISE.GridProto.CwfgmFuelMap fuels = 3;</code>
   * @return Whether the fuels field is set.
   */
  boolean hasFuels();
  /**
   * <code>.WISE.GridProto.CwfgmFuelMap fuels = 3;</code>
   * @return The fuels.
   */
  ca.wise.grid.proto.CwfgmFuelMap getFuels();
  /**
   * <code>.WISE.GridProto.CwfgmFuelMap fuels = 3;</code>
   */
  ca.wise.grid.proto.CwfgmFuelMapOrBuilder getFuelsOrBuilder();

  /**
   * <code>.WISE.GridProto.CwfgmGrid grid = 4;</code>
   * @return Whether the grid field is set.
   */
  boolean hasGrid();
  /**
   * <code>.WISE.GridProto.CwfgmGrid grid = 4;</code>
   * @return The grid.
   */
  ca.wise.grid.proto.CwfgmGrid getGrid();
  /**
   * <code>.WISE.GridProto.CwfgmGrid grid = 4;</code>
   */
  ca.wise.grid.proto.CwfgmGridOrBuilder getGridOrBuilder();

  /**
   * <code>.WISE.ProjectProto.FuelCollection fuelCollection = 5 [deprecated = true];</code>
   * @return Whether the fuelCollection field is set.
   */
  @java.lang.Deprecated boolean hasFuelCollection();
  /**
   * <code>.WISE.ProjectProto.FuelCollection fuelCollection = 5 [deprecated = true];</code>
   * @return The fuelCollection.
   */
  @java.lang.Deprecated ca.wise.project.proto.FuelCollection getFuelCollection();
  /**
   * <code>.WISE.ProjectProto.FuelCollection fuelCollection = 5 [deprecated = true];</code>
   */
  @java.lang.Deprecated ca.wise.project.proto.FuelCollectionOrBuilder getFuelCollectionOrBuilder();

  /**
   * <code>.WISE.ProjectProto.FireCollection ignitions = 6;</code>
   * @return Whether the ignitions field is set.
   */
  boolean hasIgnitions();
  /**
   * <code>.WISE.ProjectProto.FireCollection ignitions = 6;</code>
   * @return The ignitions.
   */
  ca.wise.project.proto.FireCollection getIgnitions();
  /**
   * <code>.WISE.ProjectProto.FireCollection ignitions = 6;</code>
   */
  ca.wise.project.proto.FireCollectionOrBuilder getIgnitionsOrBuilder();

  /**
   * <code>.WISE.ProjectProto.StationCollection stations = 7;</code>
   * @return Whether the stations field is set.
   */
  boolean hasStations();
  /**
   * <code>.WISE.ProjectProto.StationCollection stations = 7;</code>
   * @return The stations.
   */
  ca.wise.project.proto.StationCollection getStations();
  /**
   * <code>.WISE.ProjectProto.StationCollection stations = 7;</code>
   */
  ca.wise.project.proto.StationCollectionOrBuilder getStationsOrBuilder();

  /**
   * <code>.WISE.ProjectProto.GridCollection grids = 8;</code>
   * @return Whether the grids field is set.
   */
  boolean hasGrids();
  /**
   * <code>.WISE.ProjectProto.GridCollection grids = 8;</code>
   * @return The grids.
   */
  ca.wise.project.proto.GridCollection getGrids();
  /**
   * <code>.WISE.ProjectProto.GridCollection grids = 8;</code>
   */
  ca.wise.project.proto.GridCollectionOrBuilder getGridsOrBuilder();

  /**
   * <code>.WISE.ProjectProto.VectorCollection vectors = 9;</code>
   * @return Whether the vectors field is set.
   */
  boolean hasVectors();
  /**
   * <code>.WISE.ProjectProto.VectorCollection vectors = 9;</code>
   * @return The vectors.
   */
  ca.wise.project.proto.VectorCollection getVectors();
  /**
   * <code>.WISE.ProjectProto.VectorCollection vectors = 9;</code>
   */
  ca.wise.project.proto.VectorCollectionOrBuilder getVectorsOrBuilder();

  /**
   * <code>.WISE.ProjectProto.ScenarioCollection scenarios = 10;</code>
   * @return Whether the scenarios field is set.
   */
  boolean hasScenarios();
  /**
   * <code>.WISE.ProjectProto.ScenarioCollection scenarios = 10;</code>
   * @return The scenarios.
   */
  ca.wise.project.proto.ScenarioCollection getScenarios();
  /**
   * <code>.WISE.ProjectProto.ScenarioCollection scenarios = 10;</code>
   */
  ca.wise.project.proto.ScenarioCollectionOrBuilder getScenariosOrBuilder();

  /**
   * <pre>
   * not really deprecated for V2, just stated so it's here for updated files, not for new files
   * </pre>
   *
   * <code>.WISE.ProjectProto.ViewCollection views = 11 [deprecated = true];</code>
   * @return Whether the views field is set.
   */
  @java.lang.Deprecated boolean hasViews();
  /**
   * <pre>
   * not really deprecated for V2, just stated so it's here for updated files, not for new files
   * </pre>
   *
   * <code>.WISE.ProjectProto.ViewCollection views = 11 [deprecated = true];</code>
   * @return The views.
   */
  @java.lang.Deprecated ca.wise.project.proto.ViewCollection getViews();
  /**
   * <pre>
   * not really deprecated for V2, just stated so it's here for updated files, not for new files
   * </pre>
   *
   * <code>.WISE.ProjectProto.ViewCollection views = 11 [deprecated = true];</code>
   */
  @java.lang.Deprecated ca.wise.project.proto.ViewCollectionOrBuilder getViewsOrBuilder();

  /**
   * <code>string elevationName = 12;</code>
   * @return The elevationName.
   */
  java.lang.String getElevationName();
  /**
   * <code>string elevationName = 12;</code>
   * @return The bytes for elevationName.
   */
  com.google.protobuf.ByteString
      getElevationNameBytes();

  /**
   * <code>string gridName = 13;</code>
   * @return The gridName.
   */
  java.lang.String getGridName();
  /**
   * <code>string gridName = 13;</code>
   * @return The bytes for gridName.
   */
  com.google.protobuf.ByteString
      getGridNameBytes();

  /**
   * <code>string projectionName = 14;</code>
   * @return The projectionName.
   */
  java.lang.String getProjectionName();
  /**
   * <code>string projectionName = 14;</code>
   * @return The bytes for projectionName.
   */
  com.google.protobuf.ByteString
      getProjectionNameBytes();

  /**
   * <code>.WISE.ProjectProto.Project.Outputs outputs = 15;</code>
   * @return Whether the outputs field is set.
   */
  boolean hasOutputs();
  /**
   * <code>.WISE.ProjectProto.Project.Outputs outputs = 15;</code>
   * @return The outputs.
   */
  ca.wise.project.proto.Project.Outputs getOutputs();
  /**
   * <code>.WISE.ProjectProto.Project.Outputs outputs = 15;</code>
   */
  ca.wise.project.proto.Project.OutputsOrBuilder getOutputsOrBuilder();

  /**
   * <pre>
   * when the project was created
   * </pre>
   *
   * <code>.HSS.Times.WTime projectStartTime = 16;</code>
   * @return Whether the projectStartTime field is set.
   */
  boolean hasProjectStartTime();
  /**
   * <pre>
   * when the project was created
   * </pre>
   *
   * <code>.HSS.Times.WTime projectStartTime = 16;</code>
   * @return The projectStartTime.
   */
  ca.hss.times.proto.WTime getProjectStartTime();
  /**
   * <pre>
   * when the project was created
   * </pre>
   *
   * <code>.HSS.Times.WTime projectStartTime = 16;</code>
   */
  ca.hss.times.proto.WTimeOrBuilder getProjectStartTimeOrBuilder();

  /**
   * <code>.WISE.ProjectProto.Project.TimestepSettings timestepSettings = 17;</code>
   * @return Whether the timestepSettings field is set.
   */
  boolean hasTimestepSettings();
  /**
   * <code>.WISE.ProjectProto.Project.TimestepSettings timestepSettings = 17;</code>
   * @return The timestepSettings.
   */
  ca.wise.project.proto.Project.TimestepSettings getTimestepSettings();
  /**
   * <code>.WISE.ProjectProto.Project.TimestepSettings timestepSettings = 17;</code>
   */
  ca.wise.project.proto.Project.TimestepSettingsOrBuilder getTimestepSettingsOrBuilder();

  /**
   * <pre>
   *The type of load balancing to use
   * </pre>
   *
   * <code>.WISE.ProjectProto.Project.LoadBalancingType loadBalancing = 18;</code>
   * @return The enum numeric value on the wire for loadBalancing.
   */
  int getLoadBalancingValue();
  /**
   * <pre>
   *The type of load balancing to use
   * </pre>
   *
   * <code>.WISE.ProjectProto.Project.LoadBalancingType loadBalancing = 18;</code>
   * @return The loadBalancing.
   */
  ca.wise.project.proto.Project.LoadBalancingType getLoadBalancing();

  /**
   * <code>.WISE.ProjectProto.AssetCollection assets = 19;</code>
   * @return Whether the assets field is set.
   */
  boolean hasAssets();
  /**
   * <code>.WISE.ProjectProto.AssetCollection assets = 19;</code>
   * @return The assets.
   */
  ca.wise.project.proto.AssetCollection getAssets();
  /**
   * <code>.WISE.ProjectProto.AssetCollection assets = 19;</code>
   */
  ca.wise.project.proto.AssetCollectionOrBuilder getAssetsOrBuilder();

  /**
   * <pre>
   * explicit settings for the project's timezone
   * </pre>
   *
   * <code>.HSS.Times.WTimeZone timeZoneSettings = 20;</code>
   * @return Whether the timeZoneSettings field is set.
   */
  boolean hasTimeZoneSettings();
  /**
   * <pre>
   * explicit settings for the project's timezone
   * </pre>
   *
   * <code>.HSS.Times.WTimeZone timeZoneSettings = 20;</code>
   * @return The timeZoneSettings.
   */
  ca.hss.times.proto.WTimeZone getTimeZoneSettings();
  /**
   * <pre>
   * explicit settings for the project's timezone
   * </pre>
   *
   * <code>.HSS.Times.WTimeZone timeZoneSettings = 20;</code>
   */
  ca.hss.times.proto.WTimeZoneOrBuilder getTimeZoneSettingsOrBuilder();

  /**
   * <code>.WISE.ProjectProto.TargetCollection targets = 21;</code>
   * @return Whether the targets field is set.
   */
  boolean hasTargets();
  /**
   * <code>.WISE.ProjectProto.TargetCollection targets = 21;</code>
   * @return The targets.
   */
  ca.wise.project.proto.TargetCollection getTargets();
  /**
   * <code>.WISE.ProjectProto.TargetCollection targets = 21;</code>
   */
  ca.wise.project.proto.TargetCollectionOrBuilder getTargetsOrBuilder();
}
