// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: vectorCollection.proto

package ca.wise.project.proto;

public interface VectorCollectionOrBuilder extends
    // @@protoc_insertion_point(interface_extends:WISE.ProjectProto.VectorCollection)
    com.google.protobuf.MessageOrBuilder {

  /**
   * <code>int32 version = 1;</code>
   * @return The version.
   */
  int getVersion();

  /**
   * <code>repeated .WISE.ProjectProto.VectorCollection.Vector vectors = 2 [deprecated = true];</code>
   */
  @java.lang.Deprecated java.util.List<ca.wise.project.proto.VectorCollection.Vector> 
      getVectorsList();
  /**
   * <code>repeated .WISE.ProjectProto.VectorCollection.Vector vectors = 2 [deprecated = true];</code>
   */
  @java.lang.Deprecated ca.wise.project.proto.VectorCollection.Vector getVectors(int index);
  /**
   * <code>repeated .WISE.ProjectProto.VectorCollection.Vector vectors = 2 [deprecated = true];</code>
   */
  @java.lang.Deprecated int getVectorsCount();
  /**
   * <code>repeated .WISE.ProjectProto.VectorCollection.Vector vectors = 2 [deprecated = true];</code>
   */
  @java.lang.Deprecated java.util.List<? extends ca.wise.project.proto.VectorCollection.VectorOrBuilder> 
      getVectorsOrBuilderList();
  /**
   * <code>repeated .WISE.ProjectProto.VectorCollection.Vector vectors = 2 [deprecated = true];</code>
   */
  @java.lang.Deprecated ca.wise.project.proto.VectorCollection.VectorOrBuilder getVectorsOrBuilder(
      int index);

  /**
   * <code>repeated .WISE.GridProto.CwfgmVectorFilter vectorData = 3;</code>
   */
  java.util.List<ca.wise.grid.proto.CwfgmVectorFilter> 
      getVectorDataList();
  /**
   * <code>repeated .WISE.GridProto.CwfgmVectorFilter vectorData = 3;</code>
   */
  ca.wise.grid.proto.CwfgmVectorFilter getVectorData(int index);
  /**
   * <code>repeated .WISE.GridProto.CwfgmVectorFilter vectorData = 3;</code>
   */
  int getVectorDataCount();
  /**
   * <code>repeated .WISE.GridProto.CwfgmVectorFilter vectorData = 3;</code>
   */
  java.util.List<? extends ca.wise.grid.proto.CwfgmVectorFilterOrBuilder> 
      getVectorDataOrBuilderList();
  /**
   * <code>repeated .WISE.GridProto.CwfgmVectorFilter vectorData = 3;</code>
   */
  ca.wise.grid.proto.CwfgmVectorFilterOrBuilder getVectorDataOrBuilder(
      int index);
}
