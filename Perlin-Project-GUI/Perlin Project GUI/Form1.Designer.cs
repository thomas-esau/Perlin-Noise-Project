namespace Perlin_Project_GUI
{
	partial class Form1
	{
		/// <summary>
		/// Erforderliche Designervariable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Verwendete Ressourcen bereinigen.
		/// </summary>
		/// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Vom Windows Form-Designer generierter Code

		/// <summary>
		/// Erforderliche Methode für die Designerunterstützung.
		/// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			this.listView1 = new System.Windows.Forms.ListView();
			this.ip = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.status = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.latency = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
			this.errorProvider1 = new System.Windows.Forms.ErrorProvider(this.components);
			((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).BeginInit();
			this.SuspendLayout();
			// 
			// listView1
			// 
			this.listView1.AllowColumnReorder = true;
			this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.ip,
            this.status,
            this.latency});
			this.listView1.HideSelection = false;
			this.listView1.Location = new System.Drawing.Point(469, 12);
			this.listView1.Name = "listView1";
			this.listView1.Size = new System.Drawing.Size(319, 426);
			this.listView1.TabIndex = 0;
			this.listView1.UseCompatibleStateImageBehavior = false;
			this.listView1.View = System.Windows.Forms.View.Details;
			this.listView1.SelectedIndexChanged += new System.EventHandler(this.listView1_SelectedIndexChanged);
			// 
			// ip
			// 
			this.ip.Text = "IP";
			this.ip.Width = 120;
			// 
			// status
			// 
			this.status.Text = "Status";
			// 
			// latency
			// 
			this.latency.Text = "Latenz";
			// 
			// errorProvider1
			// 
			this.errorProvider1.ContainerControl = this;
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(800, 450);
			this.Controls.Add(this.listView1);
			this.Name = "Form1";
			this.Text = "Form1";
			((System.ComponentModel.ISupportInitialize)(this.errorProvider1)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.ListView listView1;
		private System.Windows.Forms.ColumnHeader ip;
		private System.Windows.Forms.ColumnHeader status;
		private System.Windows.Forms.ColumnHeader latency;
		private System.Windows.Forms.ErrorProvider errorProvider1;
	}
}

