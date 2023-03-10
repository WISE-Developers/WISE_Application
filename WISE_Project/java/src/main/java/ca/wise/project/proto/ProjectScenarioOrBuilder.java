// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: projectScenario.proto

package ca.wise.project.proto;

@java.lang.Deprecated public interface ProjectScenarioOrBuilder extends
    // @@protoc_insertion_point(interface_extends:WISE.ProjectProto.ProjectScenario)
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
   * <code>.google.protobuf.StringValue copyName = 3;</code>
   * @return Whether the copyName field is set.
   */
  boolean hasCopyName();
  /**
   * <code>.google.protobuf.StringValue copyName = 3;</code>
   * @return The copyName.
   */
  com.google.protobuf.StringValue getCopyName();
  /**
   * <code>.google.protobuf.StringValue copyName = 3;</code>
   */
  com.google.protobuf.StringValueOrBuilder getCopyNameOrBuilder();

  /**
   * <code>string comments = 4;</code>
   * @return The comments.
   */
  java.lang.String getComments();
  /**
   * <code>string comments = 4;</code>
   * @return The bytes for comments.
   */
  com.google.protobuf.ByteString
      getCommentsBytes();

  /**
   * <code>.WISE.FireEngineProto.CwfgmScenario scenario = 5;</code>
   * @return Whether the scenario field is set.
   */
  boolean hasScenario();
  /**
   * <code>.WISE.FireEngineProto.CwfgmScenario scenario = 5;</code>
   * @return The scenario.
   */
  ca.wise.fire.proto.CwfgmScenario getScenario();
  /**
   * <code>.WISE.FireEngineProto.CwfgmScenario scenario = 5;</code>
   */
  ca.wise.fire.proto.CwfgmScenarioOrBuilder getScenarioOrBuilder();

  /**
   * <code>.WISE.GridProto.TemporalCondition temporalConditions = 6;</code>
   * @return Whether the temporalConditions field is set.
   */
  boolean hasTemporalConditions();
  /**
   * <code>.WISE.GridProto.TemporalCondition temporalConditions = 6;</code>
   * @return The temporalConditions.
   */
  ca.wise.grid.proto.TemporalCondition getTemporalConditions();
  /**
   * <code>.WISE.GridProto.TemporalCondition temporalConditions = 6;</code>
   */
  ca.wise.grid.proto.TemporalConditionOrBuilder getTemporalConditionsOrBuilder();

  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference fireIndex = 7;</code>
   */
  java.util.List<ca.wise.project.proto.ProjectScenario.Reference> 
      getFireIndexList();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference fireIndex = 7;</code>
   */
  ca.wise.project.proto.ProjectScenario.Reference getFireIndex(int index);
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference fireIndex = 7;</code>
   */
  int getFireIndexCount();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference fireIndex = 7;</code>
   */
  java.util.List<? extends ca.wise.project.proto.ProjectScenario.ReferenceOrBuilder> 
      getFireIndexOrBuilderList();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference fireIndex = 7;</code>
   */
  ca.wise.project.proto.ProjectScenario.ReferenceOrBuilder getFireIndexOrBuilder(
      int index);

  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.WeatherIndex weatherIndex = 8;</code>
   */
  java.util.List<ca.wise.project.proto.ProjectScenario.WeatherIndex> 
      getWeatherIndexList();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.WeatherIndex weatherIndex = 8;</code>
   */
  ca.wise.project.proto.ProjectScenario.WeatherIndex getWeatherIndex(int index);
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.WeatherIndex weatherIndex = 8;</code>
   */
  int getWeatherIndexCount();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.WeatherIndex weatherIndex = 8;</code>
   */
  java.util.List<? extends ca.wise.project.proto.ProjectScenario.WeatherIndexOrBuilder> 
      getWeatherIndexOrBuilderList();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.WeatherIndex weatherIndex = 8;</code>
   */
  ca.wise.project.proto.ProjectScenario.WeatherIndexOrBuilder getWeatherIndexOrBuilder(
      int index);

  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference filterIndex = 9;</code>
   */
  java.util.List<ca.wise.project.proto.ProjectScenario.Reference> 
      getFilterIndexList();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference filterIndex = 9;</code>
   */
  ca.wise.project.proto.ProjectScenario.Reference getFilterIndex(int index);
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference filterIndex = 9;</code>
   */
  int getFilterIndexCount();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference filterIndex = 9;</code>
   */
  java.util.List<? extends ca.wise.project.proto.ProjectScenario.ReferenceOrBuilder> 
      getFilterIndexOrBuilderList();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference filterIndex = 9;</code>
   */
  ca.wise.project.proto.ProjectScenario.ReferenceOrBuilder getFilterIndexOrBuilder(
      int index);

  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference vectorIndex = 10;</code>
   */
  java.util.List<ca.wise.project.proto.ProjectScenario.Reference> 
      getVectorIndexList();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference vectorIndex = 10;</code>
   */
  ca.wise.project.proto.ProjectScenario.Reference getVectorIndex(int index);
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference vectorIndex = 10;</code>
   */
  int getVectorIndexCount();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference vectorIndex = 10;</code>
   */
  java.util.List<? extends ca.wise.project.proto.ProjectScenario.ReferenceOrBuilder> 
      getVectorIndexOrBuilderList();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference vectorIndex = 10;</code>
   */
  ca.wise.project.proto.ProjectScenario.ReferenceOrBuilder getVectorIndexOrBuilder(
      int index);

  /**
   * <code>.WISE.ProjectProto.ProjectScenario.IdwExponent idw = 11;</code>
   * @return Whether the idw field is set.
   */
  boolean hasIdw();
  /**
   * <code>.WISE.ProjectProto.ProjectScenario.IdwExponent idw = 11;</code>
   * @return The idw.
   */
  ca.wise.project.proto.ProjectScenario.IdwExponent getIdw();
  /**
   * <code>.WISE.ProjectProto.ProjectScenario.IdwExponent idw = 11;</code>
   */
  ca.wise.project.proto.ProjectScenario.IdwExponentOrBuilder getIdwOrBuilder();

  /**
   * <code>.google.protobuf.UInt32Value threadCount = 12 [deprecated = true];</code>
   * @return Whether the threadCount field is set.
   */
  @java.lang.Deprecated boolean hasThreadCount();
  /**
   * <code>.google.protobuf.UInt32Value threadCount = 12 [deprecated = true];</code>
   * @return The threadCount.
   */
  @java.lang.Deprecated com.google.protobuf.UInt32Value getThreadCount();
  /**
   * <code>.google.protobuf.UInt32Value threadCount = 12 [deprecated = true];</code>
   */
  @java.lang.Deprecated com.google.protobuf.UInt32ValueOrBuilder getThreadCountOrBuilder();

  /**
   * <code>.WISE.ProjectProto.ProjectScenario.StochasticOptions stochasticOptions = 13 [deprecated = true];</code>
   * @return Whether the stochasticOptions field is set.
   */
  @java.lang.Deprecated boolean hasStochasticOptions();
  /**
   * <code>.WISE.ProjectProto.ProjectScenario.StochasticOptions stochasticOptions = 13 [deprecated = true];</code>
   * @return The stochasticOptions.
   */
  @java.lang.Deprecated ca.wise.project.proto.ProjectScenario.StochasticOptions getStochasticOptions();
  /**
   * <code>.WISE.ProjectProto.ProjectScenario.StochasticOptions stochasticOptions = 13 [deprecated = true];</code>
   */
  @java.lang.Deprecated ca.wise.project.proto.ProjectScenario.StochasticOptionsOrBuilder getStochasticOptionsOrBuilder();

  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference assetIndex = 14;</code>
   */
  java.util.List<ca.wise.project.proto.ProjectScenario.Reference> 
      getAssetIndexList();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference assetIndex = 14;</code>
   */
  ca.wise.project.proto.ProjectScenario.Reference getAssetIndex(int index);
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference assetIndex = 14;</code>
   */
  int getAssetIndexCount();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference assetIndex = 14;</code>
   */
  java.util.List<? extends ca.wise.project.proto.ProjectScenario.ReferenceOrBuilder> 
      getAssetIndexOrBuilderList();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectScenario.Reference assetIndex = 14;</code>
   */
  ca.wise.project.proto.ProjectScenario.ReferenceOrBuilder getAssetIndexOrBuilder(
      int index);

  /**
   * <code>.WISE.ProjectProto.ProjectScenario.AssetOperation globalAssetOperation = 15;</code>
   * @return The enum numeric value on the wire for globalAssetOperation.
   */
  int getGlobalAssetOperationValue();
  /**
   * <code>.WISE.ProjectProto.ProjectScenario.AssetOperation globalAssetOperation = 15;</code>
   * @return The globalAssetOperation.
   */
  ca.wise.project.proto.ProjectScenario.AssetOperation getGlobalAssetOperation();

  /**
   * <pre>
   *the number of assets that can be collided into before a simulation will stop, if valid for the asset operation
   * </pre>
   *
   * <code>.google.protobuf.Int32Value globalCollisionCount = 16;</code>
   * @return Whether the globalCollisionCount field is set.
   */
  boolean hasGlobalCollisionCount();
  /**
   * <pre>
   *the number of assets that can be collided into before a simulation will stop, if valid for the asset operation
   * </pre>
   *
   * <code>.google.protobuf.Int32Value globalCollisionCount = 16;</code>
   * @return The globalCollisionCount.
   */
  com.google.protobuf.Int32Value getGlobalCollisionCount();
  /**
   * <pre>
   *the number of assets that can be collided into before a simulation will stop, if valid for the asset operation
   * </pre>
   *
   * <code>.google.protobuf.Int32Value globalCollisionCount = 16;</code>
   */
  com.google.protobuf.Int32ValueOrBuilder getGlobalCollisionCountOrBuilder();

  /**
   * <code>.WISE.ProjectProto.ProjectScenario.TargetReference windTarget = 17;</code>
   * @return Whether the windTarget field is set.
   */
  boolean hasWindTarget();
  /**
   * <code>.WISE.ProjectProto.ProjectScenario.TargetReference windTarget = 17;</code>
   * @return The windTarget.
   */
  ca.wise.project.proto.ProjectScenario.TargetReference getWindTarget();
  /**
   * <code>.WISE.ProjectProto.ProjectScenario.TargetReference windTarget = 17;</code>
   */
  ca.wise.project.proto.ProjectScenario.TargetReferenceOrBuilder getWindTargetOrBuilder();

  /**
   * <code>.WISE.ProjectProto.ProjectScenario.TargetReference vectorTarget = 18;</code>
   * @return Whether the vectorTarget field is set.
   */
  boolean hasVectorTarget();
  /**
   * <code>.WISE.ProjectProto.ProjectScenario.TargetReference vectorTarget = 18;</code>
   * @return The vectorTarget.
   */
  ca.wise.project.proto.ProjectScenario.TargetReference getVectorTarget();
  /**
   * <code>.WISE.ProjectProto.ProjectScenario.TargetReference vectorTarget = 18;</code>
   */
  ca.wise.project.proto.ProjectScenario.TargetReferenceOrBuilder getVectorTargetOrBuilder();

  public ca.wise.project.proto.ProjectScenario.GlobalAssetOperationNullCase getGlobalAssetOperationNullCase();
}
