// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: weatherStation.proto

package ca.wise.project.proto;

public final class WeatherStation {
  private WeatherStation() {}
  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistryLite registry) {
  }

  public static void registerAllExtensions(
      com.google.protobuf.ExtensionRegistry registry) {
    registerAllExtensions(
        (com.google.protobuf.ExtensionRegistryLite) registry);
  }
  static final com.google.protobuf.Descriptors.Descriptor
    internal_static_WISE_ProjectProto_ProjectWeatherStation_descriptor;
  static final 
    com.google.protobuf.GeneratedMessageV3.FieldAccessorTable
      internal_static_WISE_ProjectProto_ProjectWeatherStation_fieldAccessorTable;
  static final com.google.protobuf.Descriptors.Descriptor
    internal_static_WISE_ProjectProto_StationCollection_descriptor;
  static final 
    com.google.protobuf.GeneratedMessageV3.FieldAccessorTable
      internal_static_WISE_ProjectProto_StationCollection_fieldAccessorTable;

  public static com.google.protobuf.Descriptors.FileDescriptor
      getDescriptor() {
    return descriptor;
  }
  private static  com.google.protobuf.Descriptors.FileDescriptor
      descriptor;
  static {
    java.lang.String[] descriptorData = {
      "\n\024weatherStation.proto\022\021WISE.ProjectProt" +
      "o\032\031cwfgmWeatherStation.proto\032\036google/pro" +
      "tobuf/wrappers.proto\"\215\002\n\025ProjectWeatherS" +
      "tation\022\017\n\007version\030\001 \001(\005\022\014\n\004name\030\002 \001(\t\022\020\n" +
      "\010comments\030\003 \001(\t\022+\n\005color\030\004 \001(\0132\034.google." +
      "protobuf.UInt32Value\022*\n\004size\030\005 \001(\0132\034.goo" +
      "gle.protobuf.UInt64Value\022-\n\007display\030\006 \001(" +
      "\0132\034.google.protobuf.UInt32Value\0227\n\007stati" +
      "on\030\007 \001(\0132&.WISE.WeatherProto.CwfgmWeathe" +
      "rStation:\002\030\001\"\237\001\n\021StationCollection\022\017\n\007ve" +
      "rsion\030\001 \001(\005\022:\n\010stations\030\002 \003(\0132(.WISE.Pro" +
      "jectProto.ProjectWeatherStation\022=\n\rwxSta" +
      "tionData\030\003 \003(\0132&.WISE.WeatherProto.Cwfgm" +
      "WeatherStationB-\n\025ca.wise.project.protoP" +
      "\001\252\002\021WISE.ProjectProtob\006proto3"
    };
    descriptor = com.google.protobuf.Descriptors.FileDescriptor
      .internalBuildGeneratedFileFrom(descriptorData,
        new com.google.protobuf.Descriptors.FileDescriptor[] {
          ca.wise.weather.proto.CwfgmWeatherStationOuterClass.getDescriptor(),
          com.google.protobuf.WrappersProto.getDescriptor(),
        });
    internal_static_WISE_ProjectProto_ProjectWeatherStation_descriptor =
      getDescriptor().getMessageTypes().get(0);
    internal_static_WISE_ProjectProto_ProjectWeatherStation_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessageV3.FieldAccessorTable(
        internal_static_WISE_ProjectProto_ProjectWeatherStation_descriptor,
        new java.lang.String[] { "Version", "Name", "Comments", "Color", "Size", "Display", "Station", });
    internal_static_WISE_ProjectProto_StationCollection_descriptor =
      getDescriptor().getMessageTypes().get(1);
    internal_static_WISE_ProjectProto_StationCollection_fieldAccessorTable = new
      com.google.protobuf.GeneratedMessageV3.FieldAccessorTable(
        internal_static_WISE_ProjectProto_StationCollection_descriptor,
        new java.lang.String[] { "Version", "Stations", "WxStationData", });
    ca.wise.weather.proto.CwfgmWeatherStationOuterClass.getDescriptor();
    com.google.protobuf.WrappersProto.getDescriptor();
  }

  // @@protoc_insertion_point(outer_class_scope)
}
