// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: projectIgnition.proto

package ca.wise.project.proto;

public interface FireCollectionOrBuilder extends
    // @@protoc_insertion_point(interface_extends:WISE.ProjectProto.FireCollection)
    com.google.protobuf.MessageOrBuilder {

  /**
   * <code>int32 version = 1;</code>
   * @return The version.
   */
  int getVersion();

  /**
   * <code>repeated .WISE.ProjectProto.ProjectFire ignitions = 2 [deprecated = true];</code>
   */
  @java.lang.Deprecated java.util.List<ca.wise.project.proto.ProjectFire> 
      getIgnitionsList();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectFire ignitions = 2 [deprecated = true];</code>
   */
  @java.lang.Deprecated ca.wise.project.proto.ProjectFire getIgnitions(int index);
  /**
   * <code>repeated .WISE.ProjectProto.ProjectFire ignitions = 2 [deprecated = true];</code>
   */
  @java.lang.Deprecated int getIgnitionsCount();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectFire ignitions = 2 [deprecated = true];</code>
   */
  @java.lang.Deprecated java.util.List<? extends ca.wise.project.proto.ProjectFireOrBuilder> 
      getIgnitionsOrBuilderList();
  /**
   * <code>repeated .WISE.ProjectProto.ProjectFire ignitions = 2 [deprecated = true];</code>
   */
  @java.lang.Deprecated ca.wise.project.proto.ProjectFireOrBuilder getIgnitionsOrBuilder(
      int index);

  /**
   * <code>repeated .WISE.FireEngineProto.CwfgmIgnition ignitionData = 3;</code>
   */
  java.util.List<ca.wise.fire.proto.CwfgmIgnition> 
      getIgnitionDataList();
  /**
   * <code>repeated .WISE.FireEngineProto.CwfgmIgnition ignitionData = 3;</code>
   */
  ca.wise.fire.proto.CwfgmIgnition getIgnitionData(int index);
  /**
   * <code>repeated .WISE.FireEngineProto.CwfgmIgnition ignitionData = 3;</code>
   */
  int getIgnitionDataCount();
  /**
   * <code>repeated .WISE.FireEngineProto.CwfgmIgnition ignitionData = 3;</code>
   */
  java.util.List<? extends ca.wise.fire.proto.CwfgmIgnitionOrBuilder> 
      getIgnitionDataOrBuilderList();
  /**
   * <code>repeated .WISE.FireEngineProto.CwfgmIgnition ignitionData = 3;</code>
   */
  ca.wise.fire.proto.CwfgmIgnitionOrBuilder getIgnitionDataOrBuilder(
      int index);
}
