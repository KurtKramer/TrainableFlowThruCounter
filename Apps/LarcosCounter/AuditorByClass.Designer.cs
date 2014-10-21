namespace LarcosCounter
{
  partial class AuditorByClass
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose (bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose ();
      }
      base.Dispose (disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent ()
    {
      this.components = new System.ComponentModel.Container ();
      this.Count = new System.Windows.Forms.TextBox ();
      this.CountLabel = new System.Windows.Forms.Label ();
      this.ThumbnailGrid = new System.Windows.Forms.DataGridView ();
      this.Column1 = new System.Windows.Forms.DataGridViewImageColumn ();
      this.Column2 = new System.Windows.Forms.DataGridViewImageColumn ();
      this.Column3 = new System.Windows.Forms.DataGridViewImageColumn ();
      this.Column4 = new System.Windows.Forms.DataGridViewImageColumn ();
      this.Column5 = new System.Windows.Forms.DataGridViewImageColumn ();
      this.ClassButonListPanel = new System.Windows.Forms.FlowLayoutPanel ();
      this.CurrentClass = new System.Windows.Forms.TextBox ();
      this.CurrentClassLabel = new System.Windows.Forms.Label ();
      this.timer1 = new System.Windows.Forms.Timer (this.components);
      this.ClassCount = new System.Windows.Forms.TextBox ();
      this.ClassCountLabel = new System.Windows.Forms.Label ();
      this.SortOrder = new System.Windows.Forms.ComboBox ();
      this.SortOrderLabel = new System.Windows.Forms.Label ();
      ((System.ComponentModel.ISupportInitialize)(this.ThumbnailGrid)).BeginInit ();
      this.SuspendLayout ();
      // 
      // Count
      // 
      this.Count.Font = new System.Drawing.Font ("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Count.Location = new System.Drawing.Point (12, 25);
      this.Count.Name = "Count";
      this.Count.ReadOnly = true;
      this.Count.Size = new System.Drawing.Size (108, 29);
      this.Count.TabIndex = 5;
      this.Count.TabStop = false;
      this.Count.Text = "0";
      this.Count.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // CountLabel
      // 
      this.CountLabel.AutoSize = true;
      this.CountLabel.Location = new System.Drawing.Point (12, 9);
      this.CountLabel.Name = "CountLabel";
      this.CountLabel.Size = new System.Drawing.Size (35, 13);
      this.CountLabel.TabIndex = 4;
      this.CountLabel.Text = "Count";
      // 
      // ThumbnailGrid
      // 
      this.ThumbnailGrid.AllowUserToAddRows = false;
      this.ThumbnailGrid.AllowUserToDeleteRows = false;
      this.ThumbnailGrid.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.ThumbnailGrid.Columns.AddRange (new System.Windows.Forms.DataGridViewColumn[] {
            this.Column1,
            this.Column2,
            this.Column3,
            this.Column4,
            this.Column5});
      this.ThumbnailGrid.Location = new System.Drawing.Point (199, 68);
      this.ThumbnailGrid.Name = "ThumbnailGrid";
      this.ThumbnailGrid.RowHeadersVisible = false;
      this.ThumbnailGrid.ShowEditingIcon = false;
      this.ThumbnailGrid.Size = new System.Drawing.Size (772, 682);
      this.ThumbnailGrid.TabIndex = 7;
      this.ThumbnailGrid.CellContentDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler (this.ThumbnailGrid_CellContentDoubleClick);
      this.ThumbnailGrid.CellFormatting += new System.Windows.Forms.DataGridViewCellFormattingEventHandler (this.ThumbnailGrid_CellFormatting);
      this.ThumbnailGrid.DataError += new System.Windows.Forms.DataGridViewDataErrorEventHandler (this.ThumbnailGrid_DataError);
      // 
      // Column1
      // 
      this.Column1.HeaderText = "Column1";
      this.Column1.Name = "Column1";
      this.Column1.Resizable = System.Windows.Forms.DataGridViewTriState.True;
      this.Column1.Width = 150;
      // 
      // Column2
      // 
      this.Column2.HeaderText = "Column2";
      this.Column2.Name = "Column2";
      this.Column2.Width = 150;
      // 
      // Column3
      // 
      this.Column3.HeaderText = "Column3";
      this.Column3.Name = "Column3";
      this.Column3.Width = 150;
      // 
      // Column4
      // 
      this.Column4.HeaderText = "Column4";
      this.Column4.Name = "Column4";
      this.Column4.Width = 150;
      // 
      // Column5
      // 
      this.Column5.HeaderText = "Column5";
      this.Column5.Name = "Column5";
      this.Column5.Width = 150;
      // 
      // ClassButonListPanel
      // 
      this.ClassButonListPanel.AutoScroll = true;
      this.ClassButonListPanel.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
      this.ClassButonListPanel.Location = new System.Drawing.Point (12, 68);
      this.ClassButonListPanel.Name = "ClassButonListPanel";
      this.ClassButonListPanel.Size = new System.Drawing.Size (181, 682);
      this.ClassButonListPanel.TabIndex = 8;
      // 
      // CurrentClass
      // 
      this.CurrentClass.Font = new System.Drawing.Font ("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.CurrentClass.Location = new System.Drawing.Point (168, 25);
      this.CurrentClass.Name = "CurrentClass";
      this.CurrentClass.ReadOnly = true;
      this.CurrentClass.Size = new System.Drawing.Size (234, 29);
      this.CurrentClass.TabIndex = 9;
      this.CurrentClass.TabStop = false;
      this.CurrentClass.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
      // 
      // CurrentClassLabel
      // 
      this.CurrentClassLabel.AutoSize = true;
      this.CurrentClassLabel.Location = new System.Drawing.Point (171, 9);
      this.CurrentClassLabel.Name = "CurrentClassLabel";
      this.CurrentClassLabel.Size = new System.Drawing.Size (79, 13);
      this.CurrentClassLabel.TabIndex = 10;
      this.CurrentClassLabel.Text = "Current Interest";
      // 
      // timer1
      // 
      this.timer1.Tick += new System.EventHandler (this.timer1_Tick);
      // 
      // ClassCount
      // 
      this.ClassCount.Font = new System.Drawing.Font ("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ClassCount.Location = new System.Drawing.Point (408, 25);
      this.ClassCount.Name = "ClassCount";
      this.ClassCount.ReadOnly = true;
      this.ClassCount.Size = new System.Drawing.Size (108, 29);
      this.ClassCount.TabIndex = 11;
      this.ClassCount.TabStop = false;
      this.ClassCount.Text = "0";
      // 
      // ClassCountLabel
      // 
      this.ClassCountLabel.AutoSize = true;
      this.ClassCountLabel.Location = new System.Drawing.Point (411, 9);
      this.ClassCountLabel.Name = "ClassCountLabel";
      this.ClassCountLabel.Size = new System.Drawing.Size (63, 13);
      this.ClassCountLabel.TabIndex = 12;
      this.ClassCountLabel.Text = "Class Count";
      // 
      // SortOrder
      // 
      this.SortOrder.Font = new System.Drawing.Font ("Microsoft Sans Serif", 14.25F);
      this.SortOrder.FormattingEnabled = true;
      this.SortOrder.Location = new System.Drawing.Point (815, 25);
      this.SortOrder.Name = "SortOrder";
      this.SortOrder.Size = new System.Drawing.Size (156, 32);
      this.SortOrder.TabIndex = 13;
      this.SortOrder.Text = "Scan-Line";
      this.SortOrder.SelectedIndexChanged += new System.EventHandler (this.SortOrder_SelectedIndexChanged);
      // 
      // SortOrderLabel
      // 
      this.SortOrderLabel.AutoSize = true;
      this.SortOrderLabel.Location = new System.Drawing.Point (913, 9);
      this.SortOrderLabel.Name = "SortOrderLabel";
      this.SortOrderLabel.Size = new System.Drawing.Size (55, 13);
      this.SortOrderLabel.TabIndex = 14;
      this.SortOrderLabel.Text = "Sort Order";
      this.SortOrderLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
      // 
      // AuditorByClass
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (984, 762);
      this.Controls.Add (this.SortOrderLabel);
      this.Controls.Add (this.SortOrder);
      this.Controls.Add (this.ClassCountLabel);
      this.Controls.Add (this.ClassCount);
      this.Controls.Add (this.CurrentClassLabel);
      this.Controls.Add (this.CurrentClass);
      this.Controls.Add (this.ClassButonListPanel);
      this.Controls.Add (this.ThumbnailGrid);
      this.Controls.Add (this.Count);
      this.Controls.Add (this.CountLabel);
      this.Name = "AuditorByClass";
      this.Text = "AuditorByClass";
      this.Load += new System.EventHandler (this.AuditorByClass_Load);
      this.Resize += new System.EventHandler (this.AuditorByClass_Resize);
      this.ResizeEnd += new System.EventHandler (this.AuditorByClass_ResizeEnd);
      ((System.ComponentModel.ISupportInitialize)(this.ThumbnailGrid)).EndInit ();
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.TextBox Count;
    private System.Windows.Forms.Label CountLabel;
    private System.Windows.Forms.DataGridView ThumbnailGrid;
    private System.Windows.Forms.FlowLayoutPanel ClassButonListPanel;
    private System.Windows.Forms.TextBox CurrentClass;
    private System.Windows.Forms.Label CurrentClassLabel;
    private System.Windows.Forms.DataGridViewImageColumn Column1;
    private System.Windows.Forms.DataGridViewImageColumn Column2;
    private System.Windows.Forms.DataGridViewImageColumn Column3;
    private System.Windows.Forms.DataGridViewImageColumn Column4;
    private System.Windows.Forms.DataGridViewImageColumn Column5;
    private System.Windows.Forms.Timer timer1;
    private System.Windows.Forms.TextBox ClassCount;
    private System.Windows.Forms.Label ClassCountLabel;
    private System.Windows.Forms.ComboBox SortOrder;
    private System.Windows.Forms.Label SortOrderLabel;
  }
}