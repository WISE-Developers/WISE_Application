// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: projectWindGrid.proto

package ca.wise.project.proto;

/**
 * Protobuf type {@code WISE.ProjectProto.ProjectWindGrid}
 */
@java.lang.Deprecated public final class ProjectWindGrid extends
    com.google.protobuf.GeneratedMessageV3 implements
    // @@protoc_insertion_point(message_implements:WISE.ProjectProto.ProjectWindGrid)
    ProjectWindGridOrBuilder {
private static final long serialVersionUID = 0L;
  // Use ProjectWindGrid.newBuilder() to construct.
  private ProjectWindGrid(com.google.protobuf.GeneratedMessageV3.Builder<?> builder) {
    super(builder);
  }
  private ProjectWindGrid() {
    name_ = "";
    comments_ = "";
    type_ = 0;
  }

  @java.lang.Override
  @SuppressWarnings({"unused"})
  protected java.lang.Object newInstance(
      UnusedPrivateParameter unused) {
    return new ProjectWindGrid();
  }

  @java.lang.Override
  public final com.google.protobuf.UnknownFieldSet
  getUnknownFields() {
    return this.unknownFields;
  }
  private ProjectWindGrid(
      com.google.protobuf.CodedInputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    this();
    if (extensionRegistry == null) {
      throw new java.lang.NullPointerException();
    }
    com.google.protobuf.UnknownFieldSet.Builder unknownFields =
        com.google.protobuf.UnknownFieldSet.newBuilder();
    try {
      boolean done = false;
      while (!done) {
        int tag = input.readTag();
        switch (tag) {
          case 0:
            done = true;
            break;
          case 8: {

            version_ = input.readInt32();
            break;
          }
          case 18: {
            java.lang.String s = input.readStringRequireUtf8();

            name_ = s;
            break;
          }
          case 26: {
            java.lang.String s = input.readStringRequireUtf8();

            comments_ = s;
            break;
          }
          case 32: {
            int rawValue = input.readEnum();

            type_ = rawValue;
            break;
          }
          case 42: {
            ca.wise.weather.proto.WindGrid.Builder subBuilder = null;
            if (grid_ != null) {
              subBuilder = grid_.toBuilder();
            }
            grid_ = input.readMessage(ca.wise.weather.proto.WindGrid.parser(), extensionRegistry);
            if (subBuilder != null) {
              subBuilder.mergeFrom(grid_);
              grid_ = subBuilder.buildPartial();
            }

            break;
          }
          default: {
            if (!parseUnknownField(
                input, unknownFields, extensionRegistry, tag)) {
              done = true;
            }
            break;
          }
        }
      }
    } catch (com.google.protobuf.InvalidProtocolBufferException e) {
      throw e.setUnfinishedMessage(this);
    } catch (java.io.IOException e) {
      throw new com.google.protobuf.InvalidProtocolBufferException(
          e).setUnfinishedMessage(this);
    } finally {
      this.unknownFields = unknownFields.build();
      makeExtensionsImmutable();
    }
  }
  public static final com.google.protobuf.Descriptors.Descriptor
      getDescriptor() {
    return ca.wise.project.proto.ProjectWindGridOuterClass.internal_static_WISE_ProjectProto_ProjectWindGrid_descriptor;
  }

  @java.lang.Override
  protected com.google.protobuf.GeneratedMessageV3.FieldAccessorTable
      internalGetFieldAccessorTable() {
    return ca.wise.project.proto.ProjectWindGridOuterClass.internal_static_WISE_ProjectProto_ProjectWindGrid_fieldAccessorTable
        .ensureFieldAccessorsInitialized(
            ca.wise.project.proto.ProjectWindGrid.class, ca.wise.project.proto.ProjectWindGrid.Builder.class);
  }

  public static final int VERSION_FIELD_NUMBER = 1;
  private int version_;
  /**
   * <code>int32 version = 1;</code>
   * @return The version.
   */
  @java.lang.Override
  public int getVersion() {
    return version_;
  }

  public static final int NAME_FIELD_NUMBER = 2;
  private volatile java.lang.Object name_;
  /**
   * <code>string name = 2;</code>
   * @return The name.
   */
  @java.lang.Override
  public java.lang.String getName() {
    java.lang.Object ref = name_;
    if (ref instanceof java.lang.String) {
      return (java.lang.String) ref;
    } else {
      com.google.protobuf.ByteString bs = 
          (com.google.protobuf.ByteString) ref;
      java.lang.String s = bs.toStringUtf8();
      name_ = s;
      return s;
    }
  }
  /**
   * <code>string name = 2;</code>
   * @return The bytes for name.
   */
  @java.lang.Override
  public com.google.protobuf.ByteString
      getNameBytes() {
    java.lang.Object ref = name_;
    if (ref instanceof java.lang.String) {
      com.google.protobuf.ByteString b = 
          com.google.protobuf.ByteString.copyFromUtf8(
              (java.lang.String) ref);
      name_ = b;
      return b;
    } else {
      return (com.google.protobuf.ByteString) ref;
    }
  }

  public static final int COMMENTS_FIELD_NUMBER = 3;
  private volatile java.lang.Object comments_;
  /**
   * <code>string comments = 3;</code>
   * @return The comments.
   */
  @java.lang.Override
  public java.lang.String getComments() {
    java.lang.Object ref = comments_;
    if (ref instanceof java.lang.String) {
      return (java.lang.String) ref;
    } else {
      com.google.protobuf.ByteString bs = 
          (com.google.protobuf.ByteString) ref;
      java.lang.String s = bs.toStringUtf8();
      comments_ = s;
      return s;
    }
  }
  /**
   * <code>string comments = 3;</code>
   * @return The bytes for comments.
   */
  @java.lang.Override
  public com.google.protobuf.ByteString
      getCommentsBytes() {
    java.lang.Object ref = comments_;
    if (ref instanceof java.lang.String) {
      com.google.protobuf.ByteString b = 
          com.google.protobuf.ByteString.copyFromUtf8(
              (java.lang.String) ref);
      comments_ = b;
      return b;
    } else {
      return (com.google.protobuf.ByteString) ref;
    }
  }

  public static final int TYPE_FIELD_NUMBER = 4;
  private int type_;
  /**
   * <code>.WISE.WeatherProto.WindGrid.GridType type = 4;</code>
   * @return The enum numeric value on the wire for type.
   */
  @java.lang.Override public int getTypeValue() {
    return type_;
  }
  /**
   * <code>.WISE.WeatherProto.WindGrid.GridType type = 4;</code>
   * @return The type.
   */
  @java.lang.Override public ca.wise.weather.proto.WindGrid.GridType getType() {
    @SuppressWarnings("deprecation")
    ca.wise.weather.proto.WindGrid.GridType result = ca.wise.weather.proto.WindGrid.GridType.valueOf(type_);
    return result == null ? ca.wise.weather.proto.WindGrid.GridType.UNRECOGNIZED : result;
  }

  public static final int GRID_FIELD_NUMBER = 5;
  private ca.wise.weather.proto.WindGrid grid_;
  /**
   * <code>.WISE.WeatherProto.WindGrid grid = 5;</code>
   * @return Whether the grid field is set.
   */
  @java.lang.Override
  public boolean hasGrid() {
    return grid_ != null;
  }
  /**
   * <code>.WISE.WeatherProto.WindGrid grid = 5;</code>
   * @return The grid.
   */
  @java.lang.Override
  public ca.wise.weather.proto.WindGrid getGrid() {
    return grid_ == null ? ca.wise.weather.proto.WindGrid.getDefaultInstance() : grid_;
  }
  /**
   * <code>.WISE.WeatherProto.WindGrid grid = 5;</code>
   */
  @java.lang.Override
  public ca.wise.weather.proto.WindGridOrBuilder getGridOrBuilder() {
    return getGrid();
  }

  private byte memoizedIsInitialized = -1;
  @java.lang.Override
  public final boolean isInitialized() {
    byte isInitialized = memoizedIsInitialized;
    if (isInitialized == 1) return true;
    if (isInitialized == 0) return false;

    memoizedIsInitialized = 1;
    return true;
  }

  @java.lang.Override
  public void writeTo(com.google.protobuf.CodedOutputStream output)
                      throws java.io.IOException {
    if (version_ != 0) {
      output.writeInt32(1, version_);
    }
    if (!getNameBytes().isEmpty()) {
      com.google.protobuf.GeneratedMessageV3.writeString(output, 2, name_);
    }
    if (!getCommentsBytes().isEmpty()) {
      com.google.protobuf.GeneratedMessageV3.writeString(output, 3, comments_);
    }
    if (type_ != ca.wise.weather.proto.WindGrid.GridType.WindSpeed.getNumber()) {
      output.writeEnum(4, type_);
    }
    if (grid_ != null) {
      output.writeMessage(5, getGrid());
    }
    unknownFields.writeTo(output);
  }

  @java.lang.Override
  public int getSerializedSize() {
    int size = memoizedSize;
    if (size != -1) return size;

    size = 0;
    if (version_ != 0) {
      size += com.google.protobuf.CodedOutputStream
        .computeInt32Size(1, version_);
    }
    if (!getNameBytes().isEmpty()) {
      size += com.google.protobuf.GeneratedMessageV3.computeStringSize(2, name_);
    }
    if (!getCommentsBytes().isEmpty()) {
      size += com.google.protobuf.GeneratedMessageV3.computeStringSize(3, comments_);
    }
    if (type_ != ca.wise.weather.proto.WindGrid.GridType.WindSpeed.getNumber()) {
      size += com.google.protobuf.CodedOutputStream
        .computeEnumSize(4, type_);
    }
    if (grid_ != null) {
      size += com.google.protobuf.CodedOutputStream
        .computeMessageSize(5, getGrid());
    }
    size += unknownFields.getSerializedSize();
    memoizedSize = size;
    return size;
  }

  @java.lang.Override
  public boolean equals(final java.lang.Object obj) {
    if (obj == this) {
     return true;
    }
    if (!(obj instanceof ca.wise.project.proto.ProjectWindGrid)) {
      return super.equals(obj);
    }
    ca.wise.project.proto.ProjectWindGrid other = (ca.wise.project.proto.ProjectWindGrid) obj;

    if (getVersion()
        != other.getVersion()) return false;
    if (!getName()
        .equals(other.getName())) return false;
    if (!getComments()
        .equals(other.getComments())) return false;
    if (type_ != other.type_) return false;
    if (hasGrid() != other.hasGrid()) return false;
    if (hasGrid()) {
      if (!getGrid()
          .equals(other.getGrid())) return false;
    }
    if (!unknownFields.equals(other.unknownFields)) return false;
    return true;
  }

  @java.lang.Override
  public int hashCode() {
    if (memoizedHashCode != 0) {
      return memoizedHashCode;
    }
    int hash = 41;
    hash = (19 * hash) + getDescriptor().hashCode();
    hash = (37 * hash) + VERSION_FIELD_NUMBER;
    hash = (53 * hash) + getVersion();
    hash = (37 * hash) + NAME_FIELD_NUMBER;
    hash = (53 * hash) + getName().hashCode();
    hash = (37 * hash) + COMMENTS_FIELD_NUMBER;
    hash = (53 * hash) + getComments().hashCode();
    hash = (37 * hash) + TYPE_FIELD_NUMBER;
    hash = (53 * hash) + type_;
    if (hasGrid()) {
      hash = (37 * hash) + GRID_FIELD_NUMBER;
      hash = (53 * hash) + getGrid().hashCode();
    }
    hash = (29 * hash) + unknownFields.hashCode();
    memoizedHashCode = hash;
    return hash;
  }

  public static ca.wise.project.proto.ProjectWindGrid parseFrom(
      java.nio.ByteBuffer data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static ca.wise.project.proto.ProjectWindGrid parseFrom(
      java.nio.ByteBuffer data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static ca.wise.project.proto.ProjectWindGrid parseFrom(
      com.google.protobuf.ByteString data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static ca.wise.project.proto.ProjectWindGrid parseFrom(
      com.google.protobuf.ByteString data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static ca.wise.project.proto.ProjectWindGrid parseFrom(byte[] data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static ca.wise.project.proto.ProjectWindGrid parseFrom(
      byte[] data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static ca.wise.project.proto.ProjectWindGrid parseFrom(java.io.InputStream input)
      throws java.io.IOException {
    return com.google.protobuf.GeneratedMessageV3
        .parseWithIOException(PARSER, input);
  }
  public static ca.wise.project.proto.ProjectWindGrid parseFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return com.google.protobuf.GeneratedMessageV3
        .parseWithIOException(PARSER, input, extensionRegistry);
  }
  public static ca.wise.project.proto.ProjectWindGrid parseDelimitedFrom(java.io.InputStream input)
      throws java.io.IOException {
    return com.google.protobuf.GeneratedMessageV3
        .parseDelimitedWithIOException(PARSER, input);
  }
  public static ca.wise.project.proto.ProjectWindGrid parseDelimitedFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return com.google.protobuf.GeneratedMessageV3
        .parseDelimitedWithIOException(PARSER, input, extensionRegistry);
  }
  public static ca.wise.project.proto.ProjectWindGrid parseFrom(
      com.google.protobuf.CodedInputStream input)
      throws java.io.IOException {
    return com.google.protobuf.GeneratedMessageV3
        .parseWithIOException(PARSER, input);
  }
  public static ca.wise.project.proto.ProjectWindGrid parseFrom(
      com.google.protobuf.CodedInputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return com.google.protobuf.GeneratedMessageV3
        .parseWithIOException(PARSER, input, extensionRegistry);
  }

  @java.lang.Override
  public Builder newBuilderForType() { return newBuilder(); }
  public static Builder newBuilder() {
    return DEFAULT_INSTANCE.toBuilder();
  }
  public static Builder newBuilder(ca.wise.project.proto.ProjectWindGrid prototype) {
    return DEFAULT_INSTANCE.toBuilder().mergeFrom(prototype);
  }
  @java.lang.Override
  public Builder toBuilder() {
    return this == DEFAULT_INSTANCE
        ? new Builder() : new Builder().mergeFrom(this);
  }

  @java.lang.Override
  protected Builder newBuilderForType(
      com.google.protobuf.GeneratedMessageV3.BuilderParent parent) {
    Builder builder = new Builder(parent);
    return builder;
  }
  /**
   * Protobuf type {@code WISE.ProjectProto.ProjectWindGrid}
   */
  public static final class Builder extends
      com.google.protobuf.GeneratedMessageV3.Builder<Builder> implements
      // @@protoc_insertion_point(builder_implements:WISE.ProjectProto.ProjectWindGrid)
      ca.wise.project.proto.ProjectWindGridOrBuilder {
    public static final com.google.protobuf.Descriptors.Descriptor
        getDescriptor() {
      return ca.wise.project.proto.ProjectWindGridOuterClass.internal_static_WISE_ProjectProto_ProjectWindGrid_descriptor;
    }

    @java.lang.Override
    protected com.google.protobuf.GeneratedMessageV3.FieldAccessorTable
        internalGetFieldAccessorTable() {
      return ca.wise.project.proto.ProjectWindGridOuterClass.internal_static_WISE_ProjectProto_ProjectWindGrid_fieldAccessorTable
          .ensureFieldAccessorsInitialized(
              ca.wise.project.proto.ProjectWindGrid.class, ca.wise.project.proto.ProjectWindGrid.Builder.class);
    }

    // Construct using ca.wise.project.proto.ProjectWindGrid.newBuilder()
    private Builder() {
      maybeForceBuilderInitialization();
    }

    private Builder(
        com.google.protobuf.GeneratedMessageV3.BuilderParent parent) {
      super(parent);
      maybeForceBuilderInitialization();
    }
    private void maybeForceBuilderInitialization() {
      if (com.google.protobuf.GeneratedMessageV3
              .alwaysUseFieldBuilders) {
      }
    }
    @java.lang.Override
    public Builder clear() {
      super.clear();
      version_ = 0;

      name_ = "";

      comments_ = "";

      type_ = 0;

      if (gridBuilder_ == null) {
        grid_ = null;
      } else {
        grid_ = null;
        gridBuilder_ = null;
      }
      return this;
    }

    @java.lang.Override
    public com.google.protobuf.Descriptors.Descriptor
        getDescriptorForType() {
      return ca.wise.project.proto.ProjectWindGridOuterClass.internal_static_WISE_ProjectProto_ProjectWindGrid_descriptor;
    }

    @java.lang.Override
    public ca.wise.project.proto.ProjectWindGrid getDefaultInstanceForType() {
      return ca.wise.project.proto.ProjectWindGrid.getDefaultInstance();
    }

    @java.lang.Override
    public ca.wise.project.proto.ProjectWindGrid build() {
      ca.wise.project.proto.ProjectWindGrid result = buildPartial();
      if (!result.isInitialized()) {
        throw newUninitializedMessageException(result);
      }
      return result;
    }

    @java.lang.Override
    public ca.wise.project.proto.ProjectWindGrid buildPartial() {
      ca.wise.project.proto.ProjectWindGrid result = new ca.wise.project.proto.ProjectWindGrid(this);
      result.version_ = version_;
      result.name_ = name_;
      result.comments_ = comments_;
      result.type_ = type_;
      if (gridBuilder_ == null) {
        result.grid_ = grid_;
      } else {
        result.grid_ = gridBuilder_.build();
      }
      onBuilt();
      return result;
    }

    @java.lang.Override
    public Builder clone() {
      return super.clone();
    }
    @java.lang.Override
    public Builder setField(
        com.google.protobuf.Descriptors.FieldDescriptor field,
        java.lang.Object value) {
      return super.setField(field, value);
    }
    @java.lang.Override
    public Builder clearField(
        com.google.protobuf.Descriptors.FieldDescriptor field) {
      return super.clearField(field);
    }
    @java.lang.Override
    public Builder clearOneof(
        com.google.protobuf.Descriptors.OneofDescriptor oneof) {
      return super.clearOneof(oneof);
    }
    @java.lang.Override
    public Builder setRepeatedField(
        com.google.protobuf.Descriptors.FieldDescriptor field,
        int index, java.lang.Object value) {
      return super.setRepeatedField(field, index, value);
    }
    @java.lang.Override
    public Builder addRepeatedField(
        com.google.protobuf.Descriptors.FieldDescriptor field,
        java.lang.Object value) {
      return super.addRepeatedField(field, value);
    }
    @java.lang.Override
    public Builder mergeFrom(com.google.protobuf.Message other) {
      if (other instanceof ca.wise.project.proto.ProjectWindGrid) {
        return mergeFrom((ca.wise.project.proto.ProjectWindGrid)other);
      } else {
        super.mergeFrom(other);
        return this;
      }
    }

    public Builder mergeFrom(ca.wise.project.proto.ProjectWindGrid other) {
      if (other == ca.wise.project.proto.ProjectWindGrid.getDefaultInstance()) return this;
      if (other.getVersion() != 0) {
        setVersion(other.getVersion());
      }
      if (!other.getName().isEmpty()) {
        name_ = other.name_;
        onChanged();
      }
      if (!other.getComments().isEmpty()) {
        comments_ = other.comments_;
        onChanged();
      }
      if (other.type_ != 0) {
        setTypeValue(other.getTypeValue());
      }
      if (other.hasGrid()) {
        mergeGrid(other.getGrid());
      }
      this.mergeUnknownFields(other.unknownFields);
      onChanged();
      return this;
    }

    @java.lang.Override
    public final boolean isInitialized() {
      return true;
    }

    @java.lang.Override
    public Builder mergeFrom(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws java.io.IOException {
      ca.wise.project.proto.ProjectWindGrid parsedMessage = null;
      try {
        parsedMessage = PARSER.parsePartialFrom(input, extensionRegistry);
      } catch (com.google.protobuf.InvalidProtocolBufferException e) {
        parsedMessage = (ca.wise.project.proto.ProjectWindGrid) e.getUnfinishedMessage();
        throw e.unwrapIOException();
      } finally {
        if (parsedMessage != null) {
          mergeFrom(parsedMessage);
        }
      }
      return this;
    }

    private int version_ ;
    /**
     * <code>int32 version = 1;</code>
     * @return The version.
     */
    @java.lang.Override
    public int getVersion() {
      return version_;
    }
    /**
     * <code>int32 version = 1;</code>
     * @param value The version to set.
     * @return This builder for chaining.
     */
    public Builder setVersion(int value) {
      
      version_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>int32 version = 1;</code>
     * @return This builder for chaining.
     */
    public Builder clearVersion() {
      
      version_ = 0;
      onChanged();
      return this;
    }

    private java.lang.Object name_ = "";
    /**
     * <code>string name = 2;</code>
     * @return The name.
     */
    public java.lang.String getName() {
      java.lang.Object ref = name_;
      if (!(ref instanceof java.lang.String)) {
        com.google.protobuf.ByteString bs =
            (com.google.protobuf.ByteString) ref;
        java.lang.String s = bs.toStringUtf8();
        name_ = s;
        return s;
      } else {
        return (java.lang.String) ref;
      }
    }
    /**
     * <code>string name = 2;</code>
     * @return The bytes for name.
     */
    public com.google.protobuf.ByteString
        getNameBytes() {
      java.lang.Object ref = name_;
      if (ref instanceof String) {
        com.google.protobuf.ByteString b = 
            com.google.protobuf.ByteString.copyFromUtf8(
                (java.lang.String) ref);
        name_ = b;
        return b;
      } else {
        return (com.google.protobuf.ByteString) ref;
      }
    }
    /**
     * <code>string name = 2;</code>
     * @param value The name to set.
     * @return This builder for chaining.
     */
    public Builder setName(
        java.lang.String value) {
      if (value == null) {
    throw new NullPointerException();
  }
  
      name_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>string name = 2;</code>
     * @return This builder for chaining.
     */
    public Builder clearName() {
      
      name_ = getDefaultInstance().getName();
      onChanged();
      return this;
    }
    /**
     * <code>string name = 2;</code>
     * @param value The bytes for name to set.
     * @return This builder for chaining.
     */
    public Builder setNameBytes(
        com.google.protobuf.ByteString value) {
      if (value == null) {
    throw new NullPointerException();
  }
  checkByteStringIsUtf8(value);
      
      name_ = value;
      onChanged();
      return this;
    }

    private java.lang.Object comments_ = "";
    /**
     * <code>string comments = 3;</code>
     * @return The comments.
     */
    public java.lang.String getComments() {
      java.lang.Object ref = comments_;
      if (!(ref instanceof java.lang.String)) {
        com.google.protobuf.ByteString bs =
            (com.google.protobuf.ByteString) ref;
        java.lang.String s = bs.toStringUtf8();
        comments_ = s;
        return s;
      } else {
        return (java.lang.String) ref;
      }
    }
    /**
     * <code>string comments = 3;</code>
     * @return The bytes for comments.
     */
    public com.google.protobuf.ByteString
        getCommentsBytes() {
      java.lang.Object ref = comments_;
      if (ref instanceof String) {
        com.google.protobuf.ByteString b = 
            com.google.protobuf.ByteString.copyFromUtf8(
                (java.lang.String) ref);
        comments_ = b;
        return b;
      } else {
        return (com.google.protobuf.ByteString) ref;
      }
    }
    /**
     * <code>string comments = 3;</code>
     * @param value The comments to set.
     * @return This builder for chaining.
     */
    public Builder setComments(
        java.lang.String value) {
      if (value == null) {
    throw new NullPointerException();
  }
  
      comments_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>string comments = 3;</code>
     * @return This builder for chaining.
     */
    public Builder clearComments() {
      
      comments_ = getDefaultInstance().getComments();
      onChanged();
      return this;
    }
    /**
     * <code>string comments = 3;</code>
     * @param value The bytes for comments to set.
     * @return This builder for chaining.
     */
    public Builder setCommentsBytes(
        com.google.protobuf.ByteString value) {
      if (value == null) {
    throw new NullPointerException();
  }
  checkByteStringIsUtf8(value);
      
      comments_ = value;
      onChanged();
      return this;
    }

    private int type_ = 0;
    /**
     * <code>.WISE.WeatherProto.WindGrid.GridType type = 4;</code>
     * @return The enum numeric value on the wire for type.
     */
    @java.lang.Override public int getTypeValue() {
      return type_;
    }
    /**
     * <code>.WISE.WeatherProto.WindGrid.GridType type = 4;</code>
     * @param value The enum numeric value on the wire for type to set.
     * @return This builder for chaining.
     */
    public Builder setTypeValue(int value) {
      
      type_ = value;
      onChanged();
      return this;
    }
    /**
     * <code>.WISE.WeatherProto.WindGrid.GridType type = 4;</code>
     * @return The type.
     */
    @java.lang.Override
    public ca.wise.weather.proto.WindGrid.GridType getType() {
      @SuppressWarnings("deprecation")
      ca.wise.weather.proto.WindGrid.GridType result = ca.wise.weather.proto.WindGrid.GridType.valueOf(type_);
      return result == null ? ca.wise.weather.proto.WindGrid.GridType.UNRECOGNIZED : result;
    }
    /**
     * <code>.WISE.WeatherProto.WindGrid.GridType type = 4;</code>
     * @param value The type to set.
     * @return This builder for chaining.
     */
    public Builder setType(ca.wise.weather.proto.WindGrid.GridType value) {
      if (value == null) {
        throw new NullPointerException();
      }
      
      type_ = value.getNumber();
      onChanged();
      return this;
    }
    /**
     * <code>.WISE.WeatherProto.WindGrid.GridType type = 4;</code>
     * @return This builder for chaining.
     */
    public Builder clearType() {
      
      type_ = 0;
      onChanged();
      return this;
    }

    private ca.wise.weather.proto.WindGrid grid_;
    private com.google.protobuf.SingleFieldBuilderV3<
        ca.wise.weather.proto.WindGrid, ca.wise.weather.proto.WindGrid.Builder, ca.wise.weather.proto.WindGridOrBuilder> gridBuilder_;
    /**
     * <code>.WISE.WeatherProto.WindGrid grid = 5;</code>
     * @return Whether the grid field is set.
     */
    public boolean hasGrid() {
      return gridBuilder_ != null || grid_ != null;
    }
    /**
     * <code>.WISE.WeatherProto.WindGrid grid = 5;</code>
     * @return The grid.
     */
    public ca.wise.weather.proto.WindGrid getGrid() {
      if (gridBuilder_ == null) {
        return grid_ == null ? ca.wise.weather.proto.WindGrid.getDefaultInstance() : grid_;
      } else {
        return gridBuilder_.getMessage();
      }
    }
    /**
     * <code>.WISE.WeatherProto.WindGrid grid = 5;</code>
     */
    public Builder setGrid(ca.wise.weather.proto.WindGrid value) {
      if (gridBuilder_ == null) {
        if (value == null) {
          throw new NullPointerException();
        }
        grid_ = value;
        onChanged();
      } else {
        gridBuilder_.setMessage(value);
      }

      return this;
    }
    /**
     * <code>.WISE.WeatherProto.WindGrid grid = 5;</code>
     */
    public Builder setGrid(
        ca.wise.weather.proto.WindGrid.Builder builderForValue) {
      if (gridBuilder_ == null) {
        grid_ = builderForValue.build();
        onChanged();
      } else {
        gridBuilder_.setMessage(builderForValue.build());
      }

      return this;
    }
    /**
     * <code>.WISE.WeatherProto.WindGrid grid = 5;</code>
     */
    public Builder mergeGrid(ca.wise.weather.proto.WindGrid value) {
      if (gridBuilder_ == null) {
        if (grid_ != null) {
          grid_ =
            ca.wise.weather.proto.WindGrid.newBuilder(grid_).mergeFrom(value).buildPartial();
        } else {
          grid_ = value;
        }
        onChanged();
      } else {
        gridBuilder_.mergeFrom(value);
      }

      return this;
    }
    /**
     * <code>.WISE.WeatherProto.WindGrid grid = 5;</code>
     */
    public Builder clearGrid() {
      if (gridBuilder_ == null) {
        grid_ = null;
        onChanged();
      } else {
        grid_ = null;
        gridBuilder_ = null;
      }

      return this;
    }
    /**
     * <code>.WISE.WeatherProto.WindGrid grid = 5;</code>
     */
    public ca.wise.weather.proto.WindGrid.Builder getGridBuilder() {
      
      onChanged();
      return getGridFieldBuilder().getBuilder();
    }
    /**
     * <code>.WISE.WeatherProto.WindGrid grid = 5;</code>
     */
    public ca.wise.weather.proto.WindGridOrBuilder getGridOrBuilder() {
      if (gridBuilder_ != null) {
        return gridBuilder_.getMessageOrBuilder();
      } else {
        return grid_ == null ?
            ca.wise.weather.proto.WindGrid.getDefaultInstance() : grid_;
      }
    }
    /**
     * <code>.WISE.WeatherProto.WindGrid grid = 5;</code>
     */
    private com.google.protobuf.SingleFieldBuilderV3<
        ca.wise.weather.proto.WindGrid, ca.wise.weather.proto.WindGrid.Builder, ca.wise.weather.proto.WindGridOrBuilder> 
        getGridFieldBuilder() {
      if (gridBuilder_ == null) {
        gridBuilder_ = new com.google.protobuf.SingleFieldBuilderV3<
            ca.wise.weather.proto.WindGrid, ca.wise.weather.proto.WindGrid.Builder, ca.wise.weather.proto.WindGridOrBuilder>(
                getGrid(),
                getParentForChildren(),
                isClean());
        grid_ = null;
      }
      return gridBuilder_;
    }
    @java.lang.Override
    public final Builder setUnknownFields(
        final com.google.protobuf.UnknownFieldSet unknownFields) {
      return super.setUnknownFields(unknownFields);
    }

    @java.lang.Override
    public final Builder mergeUnknownFields(
        final com.google.protobuf.UnknownFieldSet unknownFields) {
      return super.mergeUnknownFields(unknownFields);
    }


    // @@protoc_insertion_point(builder_scope:WISE.ProjectProto.ProjectWindGrid)
  }

  // @@protoc_insertion_point(class_scope:WISE.ProjectProto.ProjectWindGrid)
  private static final ca.wise.project.proto.ProjectWindGrid DEFAULT_INSTANCE;
  static {
    DEFAULT_INSTANCE = new ca.wise.project.proto.ProjectWindGrid();
  }

  public static ca.wise.project.proto.ProjectWindGrid getDefaultInstance() {
    return DEFAULT_INSTANCE;
  }

  private static final com.google.protobuf.Parser<ProjectWindGrid>
      PARSER = new com.google.protobuf.AbstractParser<ProjectWindGrid>() {
    @java.lang.Override
    public ProjectWindGrid parsePartialFrom(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return new ProjectWindGrid(input, extensionRegistry);
    }
  };

  public static com.google.protobuf.Parser<ProjectWindGrid> parser() {
    return PARSER;
  }

  @java.lang.Override
  public com.google.protobuf.Parser<ProjectWindGrid> getParserForType() {
    return PARSER;
  }

  @java.lang.Override
  public ca.wise.project.proto.ProjectWindGrid getDefaultInstanceForType() {
    return DEFAULT_INSTANCE;
  }

}
