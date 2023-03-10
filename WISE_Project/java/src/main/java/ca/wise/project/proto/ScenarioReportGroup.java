// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: summary.proto

package ca.wise.project.proto;

/**
 * <pre>
 **
 * A wrapper around scenario reports so multiple can be output at once.
 * </pre>
 *
 * Protobuf type {@code WISE.ProjectProto.ScenarioReportGroup}
 */
public final class ScenarioReportGroup extends
    com.google.protobuf.GeneratedMessageV3 implements
    // @@protoc_insertion_point(message_implements:WISE.ProjectProto.ScenarioReportGroup)
    ScenarioReportGroupOrBuilder {
private static final long serialVersionUID = 0L;
  // Use ScenarioReportGroup.newBuilder() to construct.
  private ScenarioReportGroup(com.google.protobuf.GeneratedMessageV3.Builder<?> builder) {
    super(builder);
  }
  private ScenarioReportGroup() {
    reports_ = java.util.Collections.emptyList();
  }

  @java.lang.Override
  @SuppressWarnings({"unused"})
  protected java.lang.Object newInstance(
      UnusedPrivateParameter unused) {
    return new ScenarioReportGroup();
  }

  @java.lang.Override
  public final com.google.protobuf.UnknownFieldSet
  getUnknownFields() {
    return this.unknownFields;
  }
  private ScenarioReportGroup(
      com.google.protobuf.CodedInputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    this();
    if (extensionRegistry == null) {
      throw new java.lang.NullPointerException();
    }
    int mutable_bitField0_ = 0;
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
          case 10: {
            if (!((mutable_bitField0_ & 0x00000001) != 0)) {
              reports_ = new java.util.ArrayList<ca.wise.project.proto.ScenarioReport>();
              mutable_bitField0_ |= 0x00000001;
            }
            reports_.add(
                input.readMessage(ca.wise.project.proto.ScenarioReport.parser(), extensionRegistry));
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
      if (((mutable_bitField0_ & 0x00000001) != 0)) {
        reports_ = java.util.Collections.unmodifiableList(reports_);
      }
      this.unknownFields = unknownFields.build();
      makeExtensionsImmutable();
    }
  }
  public static final com.google.protobuf.Descriptors.Descriptor
      getDescriptor() {
    return ca.wise.project.proto.Summary.internal_static_WISE_ProjectProto_ScenarioReportGroup_descriptor;
  }

  @java.lang.Override
  protected com.google.protobuf.GeneratedMessageV3.FieldAccessorTable
      internalGetFieldAccessorTable() {
    return ca.wise.project.proto.Summary.internal_static_WISE_ProjectProto_ScenarioReportGroup_fieldAccessorTable
        .ensureFieldAccessorsInitialized(
            ca.wise.project.proto.ScenarioReportGroup.class, ca.wise.project.proto.ScenarioReportGroup.Builder.class);
  }

  public static final int REPORTS_FIELD_NUMBER = 1;
  private java.util.List<ca.wise.project.proto.ScenarioReport> reports_;
  /**
   * <pre>
   *Reports for different scenarios
   * </pre>
   *
   * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
   */
  @java.lang.Override
  public java.util.List<ca.wise.project.proto.ScenarioReport> getReportsList() {
    return reports_;
  }
  /**
   * <pre>
   *Reports for different scenarios
   * </pre>
   *
   * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
   */
  @java.lang.Override
  public java.util.List<? extends ca.wise.project.proto.ScenarioReportOrBuilder> 
      getReportsOrBuilderList() {
    return reports_;
  }
  /**
   * <pre>
   *Reports for different scenarios
   * </pre>
   *
   * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
   */
  @java.lang.Override
  public int getReportsCount() {
    return reports_.size();
  }
  /**
   * <pre>
   *Reports for different scenarios
   * </pre>
   *
   * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
   */
  @java.lang.Override
  public ca.wise.project.proto.ScenarioReport getReports(int index) {
    return reports_.get(index);
  }
  /**
   * <pre>
   *Reports for different scenarios
   * </pre>
   *
   * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
   */
  @java.lang.Override
  public ca.wise.project.proto.ScenarioReportOrBuilder getReportsOrBuilder(
      int index) {
    return reports_.get(index);
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
    for (int i = 0; i < reports_.size(); i++) {
      output.writeMessage(1, reports_.get(i));
    }
    unknownFields.writeTo(output);
  }

  @java.lang.Override
  public int getSerializedSize() {
    int size = memoizedSize;
    if (size != -1) return size;

    size = 0;
    for (int i = 0; i < reports_.size(); i++) {
      size += com.google.protobuf.CodedOutputStream
        .computeMessageSize(1, reports_.get(i));
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
    if (!(obj instanceof ca.wise.project.proto.ScenarioReportGroup)) {
      return super.equals(obj);
    }
    ca.wise.project.proto.ScenarioReportGroup other = (ca.wise.project.proto.ScenarioReportGroup) obj;

    if (!getReportsList()
        .equals(other.getReportsList())) return false;
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
    if (getReportsCount() > 0) {
      hash = (37 * hash) + REPORTS_FIELD_NUMBER;
      hash = (53 * hash) + getReportsList().hashCode();
    }
    hash = (29 * hash) + unknownFields.hashCode();
    memoizedHashCode = hash;
    return hash;
  }

  public static ca.wise.project.proto.ScenarioReportGroup parseFrom(
      java.nio.ByteBuffer data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static ca.wise.project.proto.ScenarioReportGroup parseFrom(
      java.nio.ByteBuffer data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static ca.wise.project.proto.ScenarioReportGroup parseFrom(
      com.google.protobuf.ByteString data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static ca.wise.project.proto.ScenarioReportGroup parseFrom(
      com.google.protobuf.ByteString data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static ca.wise.project.proto.ScenarioReportGroup parseFrom(byte[] data)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data);
  }
  public static ca.wise.project.proto.ScenarioReportGroup parseFrom(
      byte[] data,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws com.google.protobuf.InvalidProtocolBufferException {
    return PARSER.parseFrom(data, extensionRegistry);
  }
  public static ca.wise.project.proto.ScenarioReportGroup parseFrom(java.io.InputStream input)
      throws java.io.IOException {
    return com.google.protobuf.GeneratedMessageV3
        .parseWithIOException(PARSER, input);
  }
  public static ca.wise.project.proto.ScenarioReportGroup parseFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return com.google.protobuf.GeneratedMessageV3
        .parseWithIOException(PARSER, input, extensionRegistry);
  }
  public static ca.wise.project.proto.ScenarioReportGroup parseDelimitedFrom(java.io.InputStream input)
      throws java.io.IOException {
    return com.google.protobuf.GeneratedMessageV3
        .parseDelimitedWithIOException(PARSER, input);
  }
  public static ca.wise.project.proto.ScenarioReportGroup parseDelimitedFrom(
      java.io.InputStream input,
      com.google.protobuf.ExtensionRegistryLite extensionRegistry)
      throws java.io.IOException {
    return com.google.protobuf.GeneratedMessageV3
        .parseDelimitedWithIOException(PARSER, input, extensionRegistry);
  }
  public static ca.wise.project.proto.ScenarioReportGroup parseFrom(
      com.google.protobuf.CodedInputStream input)
      throws java.io.IOException {
    return com.google.protobuf.GeneratedMessageV3
        .parseWithIOException(PARSER, input);
  }
  public static ca.wise.project.proto.ScenarioReportGroup parseFrom(
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
  public static Builder newBuilder(ca.wise.project.proto.ScenarioReportGroup prototype) {
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
   * <pre>
   **
   * A wrapper around scenario reports so multiple can be output at once.
   * </pre>
   *
   * Protobuf type {@code WISE.ProjectProto.ScenarioReportGroup}
   */
  public static final class Builder extends
      com.google.protobuf.GeneratedMessageV3.Builder<Builder> implements
      // @@protoc_insertion_point(builder_implements:WISE.ProjectProto.ScenarioReportGroup)
      ca.wise.project.proto.ScenarioReportGroupOrBuilder {
    public static final com.google.protobuf.Descriptors.Descriptor
        getDescriptor() {
      return ca.wise.project.proto.Summary.internal_static_WISE_ProjectProto_ScenarioReportGroup_descriptor;
    }

    @java.lang.Override
    protected com.google.protobuf.GeneratedMessageV3.FieldAccessorTable
        internalGetFieldAccessorTable() {
      return ca.wise.project.proto.Summary.internal_static_WISE_ProjectProto_ScenarioReportGroup_fieldAccessorTable
          .ensureFieldAccessorsInitialized(
              ca.wise.project.proto.ScenarioReportGroup.class, ca.wise.project.proto.ScenarioReportGroup.Builder.class);
    }

    // Construct using ca.wise.project.proto.ScenarioReportGroup.newBuilder()
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
        getReportsFieldBuilder();
      }
    }
    @java.lang.Override
    public Builder clear() {
      super.clear();
      if (reportsBuilder_ == null) {
        reports_ = java.util.Collections.emptyList();
        bitField0_ = (bitField0_ & ~0x00000001);
      } else {
        reportsBuilder_.clear();
      }
      return this;
    }

    @java.lang.Override
    public com.google.protobuf.Descriptors.Descriptor
        getDescriptorForType() {
      return ca.wise.project.proto.Summary.internal_static_WISE_ProjectProto_ScenarioReportGroup_descriptor;
    }

    @java.lang.Override
    public ca.wise.project.proto.ScenarioReportGroup getDefaultInstanceForType() {
      return ca.wise.project.proto.ScenarioReportGroup.getDefaultInstance();
    }

    @java.lang.Override
    public ca.wise.project.proto.ScenarioReportGroup build() {
      ca.wise.project.proto.ScenarioReportGroup result = buildPartial();
      if (!result.isInitialized()) {
        throw newUninitializedMessageException(result);
      }
      return result;
    }

    @java.lang.Override
    public ca.wise.project.proto.ScenarioReportGroup buildPartial() {
      ca.wise.project.proto.ScenarioReportGroup result = new ca.wise.project.proto.ScenarioReportGroup(this);
      int from_bitField0_ = bitField0_;
      if (reportsBuilder_ == null) {
        if (((bitField0_ & 0x00000001) != 0)) {
          reports_ = java.util.Collections.unmodifiableList(reports_);
          bitField0_ = (bitField0_ & ~0x00000001);
        }
        result.reports_ = reports_;
      } else {
        result.reports_ = reportsBuilder_.build();
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
      if (other instanceof ca.wise.project.proto.ScenarioReportGroup) {
        return mergeFrom((ca.wise.project.proto.ScenarioReportGroup)other);
      } else {
        super.mergeFrom(other);
        return this;
      }
    }

    public Builder mergeFrom(ca.wise.project.proto.ScenarioReportGroup other) {
      if (other == ca.wise.project.proto.ScenarioReportGroup.getDefaultInstance()) return this;
      if (reportsBuilder_ == null) {
        if (!other.reports_.isEmpty()) {
          if (reports_.isEmpty()) {
            reports_ = other.reports_;
            bitField0_ = (bitField0_ & ~0x00000001);
          } else {
            ensureReportsIsMutable();
            reports_.addAll(other.reports_);
          }
          onChanged();
        }
      } else {
        if (!other.reports_.isEmpty()) {
          if (reportsBuilder_.isEmpty()) {
            reportsBuilder_.dispose();
            reportsBuilder_ = null;
            reports_ = other.reports_;
            bitField0_ = (bitField0_ & ~0x00000001);
            reportsBuilder_ = 
              com.google.protobuf.GeneratedMessageV3.alwaysUseFieldBuilders ?
                 getReportsFieldBuilder() : null;
          } else {
            reportsBuilder_.addAllMessages(other.reports_);
          }
        }
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
      ca.wise.project.proto.ScenarioReportGroup parsedMessage = null;
      try {
        parsedMessage = PARSER.parsePartialFrom(input, extensionRegistry);
      } catch (com.google.protobuf.InvalidProtocolBufferException e) {
        parsedMessage = (ca.wise.project.proto.ScenarioReportGroup) e.getUnfinishedMessage();
        throw e.unwrapIOException();
      } finally {
        if (parsedMessage != null) {
          mergeFrom(parsedMessage);
        }
      }
      return this;
    }
    private int bitField0_;

    private java.util.List<ca.wise.project.proto.ScenarioReport> reports_ =
      java.util.Collections.emptyList();
    private void ensureReportsIsMutable() {
      if (!((bitField0_ & 0x00000001) != 0)) {
        reports_ = new java.util.ArrayList<ca.wise.project.proto.ScenarioReport>(reports_);
        bitField0_ |= 0x00000001;
       }
    }

    private com.google.protobuf.RepeatedFieldBuilderV3<
        ca.wise.project.proto.ScenarioReport, ca.wise.project.proto.ScenarioReport.Builder, ca.wise.project.proto.ScenarioReportOrBuilder> reportsBuilder_;

    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public java.util.List<ca.wise.project.proto.ScenarioReport> getReportsList() {
      if (reportsBuilder_ == null) {
        return java.util.Collections.unmodifiableList(reports_);
      } else {
        return reportsBuilder_.getMessageList();
      }
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public int getReportsCount() {
      if (reportsBuilder_ == null) {
        return reports_.size();
      } else {
        return reportsBuilder_.getCount();
      }
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public ca.wise.project.proto.ScenarioReport getReports(int index) {
      if (reportsBuilder_ == null) {
        return reports_.get(index);
      } else {
        return reportsBuilder_.getMessage(index);
      }
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public Builder setReports(
        int index, ca.wise.project.proto.ScenarioReport value) {
      if (reportsBuilder_ == null) {
        if (value == null) {
          throw new NullPointerException();
        }
        ensureReportsIsMutable();
        reports_.set(index, value);
        onChanged();
      } else {
        reportsBuilder_.setMessage(index, value);
      }
      return this;
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public Builder setReports(
        int index, ca.wise.project.proto.ScenarioReport.Builder builderForValue) {
      if (reportsBuilder_ == null) {
        ensureReportsIsMutable();
        reports_.set(index, builderForValue.build());
        onChanged();
      } else {
        reportsBuilder_.setMessage(index, builderForValue.build());
      }
      return this;
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public Builder addReports(ca.wise.project.proto.ScenarioReport value) {
      if (reportsBuilder_ == null) {
        if (value == null) {
          throw new NullPointerException();
        }
        ensureReportsIsMutable();
        reports_.add(value);
        onChanged();
      } else {
        reportsBuilder_.addMessage(value);
      }
      return this;
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public Builder addReports(
        int index, ca.wise.project.proto.ScenarioReport value) {
      if (reportsBuilder_ == null) {
        if (value == null) {
          throw new NullPointerException();
        }
        ensureReportsIsMutable();
        reports_.add(index, value);
        onChanged();
      } else {
        reportsBuilder_.addMessage(index, value);
      }
      return this;
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public Builder addReports(
        ca.wise.project.proto.ScenarioReport.Builder builderForValue) {
      if (reportsBuilder_ == null) {
        ensureReportsIsMutable();
        reports_.add(builderForValue.build());
        onChanged();
      } else {
        reportsBuilder_.addMessage(builderForValue.build());
      }
      return this;
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public Builder addReports(
        int index, ca.wise.project.proto.ScenarioReport.Builder builderForValue) {
      if (reportsBuilder_ == null) {
        ensureReportsIsMutable();
        reports_.add(index, builderForValue.build());
        onChanged();
      } else {
        reportsBuilder_.addMessage(index, builderForValue.build());
      }
      return this;
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public Builder addAllReports(
        java.lang.Iterable<? extends ca.wise.project.proto.ScenarioReport> values) {
      if (reportsBuilder_ == null) {
        ensureReportsIsMutable();
        com.google.protobuf.AbstractMessageLite.Builder.addAll(
            values, reports_);
        onChanged();
      } else {
        reportsBuilder_.addAllMessages(values);
      }
      return this;
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public Builder clearReports() {
      if (reportsBuilder_ == null) {
        reports_ = java.util.Collections.emptyList();
        bitField0_ = (bitField0_ & ~0x00000001);
        onChanged();
      } else {
        reportsBuilder_.clear();
      }
      return this;
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public Builder removeReports(int index) {
      if (reportsBuilder_ == null) {
        ensureReportsIsMutable();
        reports_.remove(index);
        onChanged();
      } else {
        reportsBuilder_.remove(index);
      }
      return this;
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public ca.wise.project.proto.ScenarioReport.Builder getReportsBuilder(
        int index) {
      return getReportsFieldBuilder().getBuilder(index);
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public ca.wise.project.proto.ScenarioReportOrBuilder getReportsOrBuilder(
        int index) {
      if (reportsBuilder_ == null) {
        return reports_.get(index);  } else {
        return reportsBuilder_.getMessageOrBuilder(index);
      }
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public java.util.List<? extends ca.wise.project.proto.ScenarioReportOrBuilder> 
         getReportsOrBuilderList() {
      if (reportsBuilder_ != null) {
        return reportsBuilder_.getMessageOrBuilderList();
      } else {
        return java.util.Collections.unmodifiableList(reports_);
      }
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public ca.wise.project.proto.ScenarioReport.Builder addReportsBuilder() {
      return getReportsFieldBuilder().addBuilder(
          ca.wise.project.proto.ScenarioReport.getDefaultInstance());
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public ca.wise.project.proto.ScenarioReport.Builder addReportsBuilder(
        int index) {
      return getReportsFieldBuilder().addBuilder(
          index, ca.wise.project.proto.ScenarioReport.getDefaultInstance());
    }
    /**
     * <pre>
     *Reports for different scenarios
     * </pre>
     *
     * <code>repeated .WISE.ProjectProto.ScenarioReport reports = 1;</code>
     */
    public java.util.List<ca.wise.project.proto.ScenarioReport.Builder> 
         getReportsBuilderList() {
      return getReportsFieldBuilder().getBuilderList();
    }
    private com.google.protobuf.RepeatedFieldBuilderV3<
        ca.wise.project.proto.ScenarioReport, ca.wise.project.proto.ScenarioReport.Builder, ca.wise.project.proto.ScenarioReportOrBuilder> 
        getReportsFieldBuilder() {
      if (reportsBuilder_ == null) {
        reportsBuilder_ = new com.google.protobuf.RepeatedFieldBuilderV3<
            ca.wise.project.proto.ScenarioReport, ca.wise.project.proto.ScenarioReport.Builder, ca.wise.project.proto.ScenarioReportOrBuilder>(
                reports_,
                ((bitField0_ & 0x00000001) != 0),
                getParentForChildren(),
                isClean());
        reports_ = null;
      }
      return reportsBuilder_;
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


    // @@protoc_insertion_point(builder_scope:WISE.ProjectProto.ScenarioReportGroup)
  }

  // @@protoc_insertion_point(class_scope:WISE.ProjectProto.ScenarioReportGroup)
  private static final ca.wise.project.proto.ScenarioReportGroup DEFAULT_INSTANCE;
  static {
    DEFAULT_INSTANCE = new ca.wise.project.proto.ScenarioReportGroup();
  }

  public static ca.wise.project.proto.ScenarioReportGroup getDefaultInstance() {
    return DEFAULT_INSTANCE;
  }

  private static final com.google.protobuf.Parser<ScenarioReportGroup>
      PARSER = new com.google.protobuf.AbstractParser<ScenarioReportGroup>() {
    @java.lang.Override
    public ScenarioReportGroup parsePartialFrom(
        com.google.protobuf.CodedInputStream input,
        com.google.protobuf.ExtensionRegistryLite extensionRegistry)
        throws com.google.protobuf.InvalidProtocolBufferException {
      return new ScenarioReportGroup(input, extensionRegistry);
    }
  };

  public static com.google.protobuf.Parser<ScenarioReportGroup> parser() {
    return PARSER;
  }

  @java.lang.Override
  public com.google.protobuf.Parser<ScenarioReportGroup> getParserForType() {
    return PARSER;
  }

  @java.lang.Override
  public ca.wise.project.proto.ScenarioReportGroup getDefaultInstanceForType() {
    return DEFAULT_INSTANCE;
  }

}

