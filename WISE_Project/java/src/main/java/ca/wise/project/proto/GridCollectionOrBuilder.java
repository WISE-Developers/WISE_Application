// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: gridCollection.proto

package ca.wise.project.proto;

public interface GridCollectionOrBuilder extends
    // @@protoc_insertion_point(interface_extends:WISE.ProjectProto.GridCollection)
    com.google.protobuf.MessageOrBuilder {

  /**
   * <code>int32 version = 1;</code>
   * @return The version.
   */
  int getVersion();

  /**
   * <code>repeated .WISE.ProjectProto.GridCollection.Filter filters = 2;</code>
   */
  java.util.List<ca.wise.project.proto.GridCollection.Filter> 
      getFiltersList();
  /**
   * <code>repeated .WISE.ProjectProto.GridCollection.Filter filters = 2;</code>
   */
  ca.wise.project.proto.GridCollection.Filter getFilters(int index);
  /**
   * <code>repeated .WISE.ProjectProto.GridCollection.Filter filters = 2;</code>
   */
  int getFiltersCount();
  /**
   * <code>repeated .WISE.ProjectProto.GridCollection.Filter filters = 2;</code>
   */
  java.util.List<? extends ca.wise.project.proto.GridCollection.FilterOrBuilder> 
      getFiltersOrBuilderList();
  /**
   * <code>repeated .WISE.ProjectProto.GridCollection.Filter filters = 2;</code>
   */
  ca.wise.project.proto.GridCollection.FilterOrBuilder getFiltersOrBuilder(
      int index);
}
